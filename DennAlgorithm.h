#pragma once
#include "Config.h"
#include "ThreadPool.h"
#include "CostFunction.h"
#include "RandomIndices.h"
#include "DatasetLoader.h"
#include "NeuralNetwork.h"
#include "DennParameters.h"
#include "DennPopulation.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennRuntimeOutput.h"

namespace Denn
{

template< typename Network, typename Parameters >
class DennAlgorithm
{
public:
	////////////////////////////////////////////////////////////////////////
	//NN
	using LayerType      = typename Network::LayerType;
	using MatrixType     = typename Network::MatrixType;
	using ScalarType     = typename Network::ScalarType;
	using LayerList      = typename Network::LayerList;
	//DB
	using DennAlgoType   = DennAlgorithm< Network, Parameters >;
	using DataSet	     = DataSetX< ScalarType >;
	//Search space
	using DBPopulation   = DoubleBufferPopulation< Network, DataSet >;
	using Individual     = typename DoubleBufferPopulation< Network, DataSet >::Individual;
	using IndividualPtr  = typename DoubleBufferPopulation< Network, DataSet >::IndividualPtr;
	using Population     = typename DoubleBufferPopulation< Network, DataSet >::Population;
	using RandomFunction = typename DoubleBufferPopulation< Network, DataSet >::RandomFunction;
	using CostFunction   = typename DoubleBufferPopulation< Network, DataSet >::CostFunction;
	//DE parallel
	using PromiseList    = std::vector< std::future<void> >;
	//Ref mutation crossover
	using MutationPtr    = std::unique_ptr < Mutation< Parameters, Population, Individual > >;
	using CrossoverPtr   = std::unique_ptr < Crossover< Individual > >;
	//Ref mutation
	////////////////////////////////////////////////////////////////////////
	//structs utilities
	struct RestartContext
	{
		ScalarType m_last_eval;
		size_t	   m_test_count;
		size_t	   m_count;
        //
		RestartContext(
			  ScalarType last_eval  = ScalarType(0.0)
			, size_t	 test_count = 0
			, size_t	 count      = 0
		)
		{
			m_last_eval  = last_eval;
			m_test_count = test_count;
			m_count      = count;
		}
	};
	struct BestContext
	{
		IndividualPtr m_best;
		ScalarType    m_eval;
        //
		BestContext(IndividualPtr best = nullptr, IndividualPtr eval = 0)
		{
			m_best = best;
			m_eval = 0;
		}
	};
	////////////////////////////////////////////////////////////////////////
	bool jde(int target, Individual& i_final) const
	{
		//vectors
		const Population& parents  = m_population.parents();
		const Individual& i_target = *parents[target];
		//f JDE
		if (RandomIndices::random() < ScalarType(m_params.m_jde_f))   
			i_final.m_f = ScalarType(RandomIndices::random(0.0,2.0));
		else														
			i_final.m_f = i_target.m_f;
		//cr JDE
		if (RandomIndices::random() < ScalarType(m_params.m_jde_cr))   
			i_final.m_cr = ScalarType(RandomIndices::random());
		else														
			i_final.m_cr = i_target.m_cr;

		return true;
	}
	////////////////////////////////////////////////////////////////////////
	DennAlgorithm
	(
		  DataSetLoader*      dataset_loader
		, const Parameters&   params
		, const Network       nn_default
		, CostFunction		  target_function
		, RuntimeOutput::SPtr output
		, ThreadPool*		  thpool = nullptr
	) 
	{
		m_dataset_loader    = dataset_loader;
		m_default		    = std::make_shared<Individual>(m_params.m_default_f, m_params.m_default_cr, nn_default );
		m_target_function   = target_function;
		m_params			= params;
		m_output            = output;
		m_thpool			= thpool;
		//default
		switch((MutationType)m_params.m_mutation_type)
		{
			default:
			case MutationType::MT_RAND_ONE:
			m_mutation   = std::make_unique< RandOne< Parameters, Population, Individual > >(m_params);
			break;
			case MutationType::MT_BEST_ONE:
			m_mutation   = std::make_unique< BestOne< Parameters, Population, Individual > >(m_params);
			break;
		}
		switch((CrossoverType)m_params.m_crossover_type)
		{
			default:
			case CrossoverType::CT_BIN:
			m_crossover   = std::make_unique< Bin<Individual> >();
			break;
			case CrossoverType::CT_EXP:
			m_crossover   = std::make_unique< Bin<Individual> >();
			break;
		}
		//init all
		reset();
	}	
	
	//init
	bool reset()
	{
		//success flag
		bool success = m_dataset_loader != nullptr;
		//init db
		success &= m_dataset_loader->start_read_batch();
		//batch
		success &= m_dataset_loader->read_batch(m_dataset_batch);
		//if success //init pop
		if (success)
		{
			m_population.init
			(
				m_params.m_np, 
				m_default, 
				m_dataset_batch,
				get_random_func(),
				m_target_function
			);
		}
		//true
		return success;
	}
	
	//big loop
	virtual IndividualPtr execute()
	{
		//global info
		const size_t n_global_pass = ((size_t)m_params.m_generations /(size_t)m_params.m_sub_gens);
		const size_t n_sub_pass = m_params.m_sub_gens;
		//restart init
		RestartContext ctx_restart;
		//best
		BestContext ctx_best(m_default->copy());
		//start output
		if (m_output->is_enable()) m_output->send_start(n_global_pass, n_sub_pass);
		//main loop
		for (size_t pass = 0; pass != n_global_pass; ++pass)
		{		
			execute_a_pass(pass, n_sub_pass, ctx_best, ctx_restart);
			//next
			next_batch();
		}
		//end output
		if (m_output->is_enable()) m_output->send_end(double(execute_test(*(ctx_best.m_best))));
		//result
		return ctx_best.m_best;
	}

	//find best individual (validation test)
	bool find_best(size_t& out_i, ScalarType& out_eval)
	{
		//ref to pop
		auto population = m_population.parents();
		//validation
		DataSet validation;
		m_dataset_loader->read_validation(validation);
		//best
		ScalarType best_eval;
		size_t	   best_i;
		//find best
		for (size_t i = 0; i != population.size(); ++i)
		{
			auto y          = population[i]->m_network.apply(validation.m_features);
			ScalarType eval = Denn::CostFunction::accuracy(validation.m_labels, y);
			//maximize (accuracy)
			if (!i || best_eval < eval)
			{
				best_i = i;
				best_eval = eval;
			}
		}
		out_i = best_i;
		out_eval = best_eval;
		return true;

	}
	
	//find best individual (validation test)
	IndividualPtr find_best(ScalarType& out_eval)
	{
		//best 
		size_t best_i;
		find_best(best_i, out_eval);
		//return best
		return m_population.parents()[best_i];
	}

	//using the test set on a individual
	ScalarType execute_test(Individual& individual)
	{
		//validation
		DataSet test;
		m_dataset_loader->read_test(test);
		//compute
		auto y = individual.m_network.apply(test.m_features);
		ScalarType eval = Denn::CostFunction::accuracy(test.m_labels, y);
		//return
		return eval;
	}

protected:
	/////////////////////////////////////////////////////////////////
	//Intermedie steps
	virtual void execute_a_pass(size_t pass,size_t n_sub_pass, BestContext& ctx_best, RestartContext& ctx_restart)
	{
		execute_target_function_on_all_population();
		//sub pass
		for (size_t sub_pass = 0; sub_pass != n_sub_pass; ++sub_pass)
		{
			execute_a_sub_pass(pass, sub_pass);
		}
		//update context
		execute_update_best(ctx_best);
		//restart
		execute_update_restart(pass, ctx_best, ctx_restart);
		//output
		if (m_output->is_enable_best())
		{
			m_output->send_best
			(
			  pass
			, ctx_restart.m_count
			, double(ctx_best.m_eval)
			, double(ctx_best.m_best->m_eval)
			);
		} 
	}
	void execute_a_sub_pass(size_t pass,size_t sub_pass)
	{
		//pass
		execute_pass();
		//output
		if (m_output->is_enable_pass())
		{
			size_t id_best;
			ScalarType val_best;
			m_population.parents().best(id_best, val_best);
			m_output->sent_pass(pass, sub_pass, id_best, val_best);
		}
	}
	void  execute_update_best(BestContext& ctx_best)
	{
		//find best
		ScalarType curr_eval;
		auto curr = find_best(curr_eval);
		//maximize (accuracy)
		if (ctx_best.m_eval < curr_eval)
		{
			//must copy because "restart" 
			//not copy element then 
			//it can change the values of the best individual
			ctx_best.m_best	= curr->copy();
			//save eval (on validation) of best
			ctx_best.m_eval = curr_eval;
		}
	}
	void  execute_update_restart(size_t pass, const BestContext& ctx_best, RestartContext& ctx)
	{
		//inc count
		if ((ctx_best.m_eval - ctx.m_last_eval) <= (ScalarType)m_params.m_restart_delta)
		{
			++ctx.m_test_count;
		}
		else
		{
			ctx.m_test_count = 0;
			ctx.m_last_eval = ctx_best.m_eval;
		}
		//first
		if (!pass) ctx.m_last_eval = ctx_best.m_eval;
		//test
		if ((ScalarType)m_params.m_restart_count <= ctx.m_test_count)
		{
			m_population.restart
			(
				  ctx_best.m_best
				, m_default
				, m_dataset_batch
				, get_random_func()
				, m_target_function
			);
			ctx.m_test_count = 0;
			ctx.m_last_eval = ctx_best.m_eval;
			//restart inc
			++ctx.m_count;
		}
	}
	/////////////////////////////////////////////////////////////////
	//execute a pass
	void execute_pass()
	{
		if (m_thpool) parallel_execute_pass(*m_thpool); 
		else          serial_execute_pass();
	}
	void serial_execute_pass()
	{
		//ref to parents
		auto& parents = m_population.parents();
		//ref to sons
		auto& sons = m_population.sons();
		//for all
		for(size_t i = 0; i!= (size_t)m_params.m_np; ++i)
		{
			execute_generation_task(i);
		}		
		//swap
		m_population.the_best_sons_become_parents();
	}
	void parallel_execute_pass(ThreadPool& thpool)
	{
		//alloc promises
		m_promises.resize(m_params.m_np);
		//execute
		for (size_t i = 0; i != (size_t)m_params.m_np; ++i)
		{
			//add
			m_promises[i] = thpool.push_task([this,i]()
			{ 
				execute_generation_task(i);
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
		//swap
		m_population.the_best_sons_become_parents();
	}
	void  execute_generation_task(size_t i)
	{
		//ref to parents
		auto& parents = m_population.parents();
		//ref to sons
		auto& sons = m_population.sons();
		//get temp individual
		auto& new_son = sons[i];
		//Copy default params
		new_son->copy_attributes(*m_default);
		//compute jde
		jde(i, *new_son);
		//call muation
		(*m_mutation)(parents, i, *new_son);
		//call crossover
		(*m_crossover)(*parents[i], *new_son);
		//eval
		auto y          = new_son->m_network.apply(m_dataset_batch.m_features);
		new_son->m_eval = m_target_function(m_dataset_batch.m_labels, y);
	}
	/////////////////////////////////////////////////////////////////
	//eval all
	void execute_target_function_on_all_population()
	{
		//eval on batch
		if (m_thpool) parallel_execute_target_function_on_all_population(*m_thpool); 
		else 		  serial_execute_target_function_on_all_population();
	}
	void serial_execute_target_function_on_all_population()
	{
		//np
		size_t np = (size_t)m_params.m_np;
		//pop ref
		auto& popolation = m_population.parents();
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			//ref to target
			auto& i_target = *popolation[i];
			//eval
			auto y = i_target.m_network.apply(m_dataset_batch.m_features);
			i_target.m_eval = m_target_function(m_dataset_batch.m_labels, y);
			//safe
			if(std::isnan(i_target.m_eval)) 
				i_target.m_eval = std::numeric_limits<ScalarType>::max() ; 
		}
	}
	void parallel_execute_target_function_on_all_population(ThreadPool& thpool)
	{
		//np
		size_t np = (size_t)m_params.m_np;
		//pop ref
		auto& popolation = m_population.parents();
		//alloc promises
		m_promises.resize(np);
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			//ref to target
			auto& i_target = *popolation[i];
			//add
			m_promises[i] = thpool.push_task([this, &i_target]()
			{
				//test
				auto y = i_target.m_network.apply(m_dataset_batch.m_features);
				i_target.m_eval = m_target_function(m_dataset_batch.m_labels, y);
				//safe
				if (std::isnan(i_target.m_eval))
					i_target.m_eval = std::numeric_limits<ScalarType>::max() ; 
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
	}
	/////////////////////////////////////////////////////////////////
	//get random function
	RandomFunction get_random_func() const
	{
		ScalarType min = m_params.m_range_min;
		ScalarType max = m_params.m_range_max;
		return [=](ScalarType x) -> ScalarType
		{
			return ScalarType(RandomIndices::random(min,max));
		};
	}
	//load next batch
	bool next_batch()
	{
		return m_dataset_loader->read_batch(m_dataset_batch);
	}
	/////////////////////////////////////////////////////////////////
	//serach space
	DBPopulation        m_population;
	PromiseList	        m_promises;
	RuntimeOutput::SPtr m_output;
	CostFunction        m_target_function;
	//dataset
	typename Individual::SPtr  m_default;
	DataSetLoader*		       m_dataset_loader;
	DataSet				       m_dataset_batch;
	//threads
	ThreadPool*				   m_thpool;
	//params of DE
	Parameters 				   m_params;
	MutationPtr                m_mutation;
	CrossoverPtr               m_crossover;
};

}