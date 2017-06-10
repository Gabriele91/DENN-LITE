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

namespace Denn
{

class RuntimeOutput : public std::enable_shared_from_this< RuntimeOutput >
{
protected:

    struct PassInfo
	{
		size_t m_n_g_pass{ 0 };
		size_t m_n_s_pass{ 0 };
		size_t m_g_pass{ 0 };
		size_t m_s_pass{ 0 };
		size_t m_minimum_on_pop_id  { 0 };
		double m_minimum_on_pop_eval{ 0 };
	};
	struct GlobalPassInfo
	{
		size_t m_g_pass{ 0 };
		size_t m_n_restart{ 0 };
		double m_validation_eval{ 0.0 };
		double m_target_eval{ 0.0 };
	};

	PassInfo m_pass;
	GlobalPassInfo m_global_pass;
	std::ostream& m_stream;

public:
	using SPtr = std::shared_ptr<RuntimeOutput>;

	RuntimeOutput(std::ostream& stream=std::cerr):m_stream(stream) {}

	SPtr get_ptr(){ return shared_from_this(); }

	virtual bool is_enable()	         { return true;     }
	virtual std::ostream& output() const { return m_stream; }

	virtual void start()
	{
		//none
	}

	virtual void update_best()
	{
		//none
	}

	virtual void update_pass()
	{
		//clean line
		for(short i=0;i!=10;++i) output() << "\t";
		output() << "\r";
		//output
		write_output();
		output() << "\r";
 	}

	virtual void end(double test_result)
	{ 
		output() << std::endl;
		output() << "Test: " << test_result;
		output() << std::endl;
	}

	virtual void send_best(
		  size_t g_pass
		, size_t n_restart
		, double validation_eval
		, double target_eval
	)
	{
		m_global_pass.m_g_pass = g_pass;
		m_global_pass.m_n_restart = n_restart;
		m_global_pass.m_validation_eval = validation_eval;
		m_global_pass.m_target_eval = target_eval;
		//
		update_best();
	}

	virtual void sent_pass(
		  size_t n_g_pass
		, size_t n_s_pass
		, size_t g_pass
		, size_t s_pass
		, size_t minimum_on_pop_id
		, double minimum_on_pop_eval
	)
	{
		m_pass.m_n_g_pass = n_g_pass;
		m_pass.m_n_s_pass = n_s_pass;
		m_pass.m_g_pass = g_pass;
		m_pass.m_s_pass = s_pass;
		m_pass.m_minimum_on_pop_id = minimum_on_pop_id;
		m_pass.m_minimum_on_pop_eval = minimum_on_pop_eval;
		//
		update_pass();
	}

	virtual void write_output() const
	{
		output()
			<< "pass: "
			<< (m_pass.m_n_s_pass * m_pass.m_g_pass + m_pass.m_s_pass)
			<< " <- (" << m_pass.m_g_pass << ", " << m_pass.m_s_pass << ")"
			<< ", population["
			<< m_pass.m_minimum_on_pop_id
			<< "] = "
			<< m_pass.m_minimum_on_pop_eval
			<< " cross entropy | restart = " 
			<< m_global_pass.m_n_restart
			<< " | best[ global pass "
					<< m_global_pass.m_g_pass
					<<", accuracy "
					<< m_global_pass.m_validation_eval
					<< ", cross entropy " 
					<< m_global_pass.m_target_eval
					<< " ]";
	}

};


template< typename Network, typename Parameters, typename DataSetLoader >
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
	using DennAlgoType   = DennAlgorithm< Network, Parameters, DataSetLoader >;
	using DataSet	     = DataSetRaw< ScalarType >;
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
	bool jde(int target, Individual& i_final) const
	{
		//vectors
		const Population& population = m_population.current();
		const Individual& i_target   = *population[target];
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
		, MutationType        mutation_type
		, CrossOverType       crossover_type
		, RuntimeOutput::SPtr output
	) 
	{
		m_dataset_loader    = dataset_loader;
		m_default		    = std::make_shared<Individual>(m_params.m_default_f, m_params.m_default_cr, nn_default );
		m_target_function   = target_function;
		m_params			= params;
		m_output            = output;
		//default
		switch(mutation_type)
		{
			default:
			case MutationType::MT_RAND_ONE:
			m_muation   = std::make_unique< RandOne< Parameters, Population, Individual > >(m_params);
			break;
			case MutationType::MT_BEST_ONE:
			m_muation   = std::make_unique< BestOne< Parameters, Population, Individual > >(m_params);
			break;
		}
		switch(crossover_type)
		{
			default:
			case CrossOverType::CR_BIN:
			m_crossover   = std::make_unique< Bin<Individual> >();
			break;
			case CrossOverType::CR_EXP:
			m_crossover   = std::make_unique< Bin<Individual> >();
			break;
		}
	}	
	
	//init
	bool init()
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
	
	//execute a pass
	void serial_pass()
	{
		//ref to population
		auto& population = m_population.current();
		//ref to next
		auto& population_next = m_population.next();
		//for all
		for(size_t i = 0; i!= (size_t)m_params.m_np; ++i)
		{
			//get temp individual
			auto& new_individual = population_next[i];
			//Copy default params
			new_individual->copy_attributes(*m_default);
			//compute jde
			jde(i, *new_individual);
			//call muation
			(*m_muation)(population, i, *new_individual);
			//call crossover
			(*m_crossover)(*population[i], *new_individual);
			//eval
			auto y                 = new_individual->m_network.apply(m_dataset_batch.m_features);
			new_individual->m_eval = m_target_function(m_dataset_batch.m_labels, y);
			//minimixe (cross_entropy)
			if (!(new_individual->m_eval < population[i]->m_eval))
			{
				//fail, next element is the target
				auto individual_tmp= population_next[i];
				population_next[i] = population[i];
				population[i]      = individual_tmp;
			}
		}
		//swap
		m_population.swap();
        //test of new generation 
        #if 0
		for(auto c_i : m_population.current())
		for(auto n_i : m_population.next())
		{
			assert(c_i.get()!=n_i.get());
		}
		#endif
	}

	//execute a pass
	void parallel_pass(ThreadPool& thpool)
	{
		//alloc promises
		m_promises.resize(m_params.m_np);
		//execute
		for (size_t i = 0; i != (size_t)m_params.m_np; ++i)
		{
			//add
			m_promises[i] = thpool.push_task([this,i]()
			{ 
				//ref to population
				auto& population = m_population.current();
				//ref to next
				auto& population_next = m_population.next();
				//get temp individual
			    auto& new_individual = population_next[i];
			    //Copy default params
			    new_individual->copy_attributes(*m_default);
				//compute jde
				jde(i, *new_individual);
				//call muation
				(*m_muation)(population, i, *new_individual);
				//call crossover
				(*m_crossover)(*population[i], *new_individual);
				//eval
				auto y                 = new_individual->m_network.apply(m_dataset_batch.m_features);
				new_individual->m_eval = m_target_function(m_dataset_batch.m_labels, y);
				//minimixe (cross_entropy)
				if (!(new_individual->m_eval < population[i]->m_eval))
				{
					//fail, next element is the target
				   auto individual_tmp= population_next[i];
				   population_next[i] = population[i];
				   population[i]      = individual_tmp;
				}
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
		//swap
		m_population.swap();
        //test of new generation 
        #if 0
		for(auto c_i : m_population.current())
		for(auto n_i : m_population.next())
		{
			assert(c_i.get()!=n_i.get());
		}
		#endif
	}

	//load next batch
	bool next_batch()
	{
		return m_dataset_loader->read_batch(m_dataset_batch);
	}

	//big loop
	IndividualPtr execute(ThreadPool* thpool = nullptr)
	{
		const size_t n_global_pass = ((size_t)m_params.m_generations /(size_t)m_params.m_sub_gens);
		const size_t n_sub_pass = m_params.m_sub_gens;
		//restart init
		ScalarType restart_last_eval = 0;
		size_t	   restart_test_count = 0;
		size_t	   restart_count = 0;
		//best
		auto best = m_default->copy();
		ScalarType best_eval = 0;
		//start output
		if (m_output->is_enable())
		{
			m_output->start();
			m_output->send_best(0, restart_count, 0.0, double(best->m_eval));
		}
		//main loop
		for (size_t pass = 0; pass != n_global_pass; ++pass)
		{		
			//eval on batch
			if (thpool)
				parallel_execute_target_function_on_all_population(*thpool); //nan in linux/g++?
			else
				serial_execute_target_function_on_all_population();

			#if 0
			for(auto& i : m_population.current()) assert(!std::isnan(i->m_eval));
			#endif
			
			//sub pass
			for (size_t sub_pass = 0; sub_pass != n_sub_pass; ++sub_pass)
			{
				if (thpool)	
					parallel_pass(*thpool); 
				else 
					serial_pass();
				//output
				if (m_output->is_enable())
				{
					size_t id_best;
					ScalarType val_best;
					m_population.current().best(id_best, val_best);
					m_output->sent_pass(n_global_pass, n_sub_pass, pass, sub_pass, id_best, val_best);
				}
			}
			//find best
			ScalarType curr_eval;
			auto curr = find_best(curr_eval);
			//maximize (accuracy)
			if (best_eval < curr_eval)
			{
				//must copy because "restart" 
				//not copy element then 
				//it can change the values of the best individual
				best	  = curr->copy();
				//save eval (on validation) of best
				best_eval = curr_eval;
			}
			//restart
			if (m_params.m_restart_enable)
			{
				//first
				if (!pass) restart_last_eval = best_eval;
				//inc count
				if ((best_eval - restart_last_eval) < (ScalarType)m_params.m_restart_delta)
				{
					++restart_test_count;
				}
				else
				{
					restart_test_count = 0;
					restart_last_eval = best_eval;
				}
				//test
				if ((ScalarType)m_params.m_restart_count <= restart_test_count)
				{
					m_population.restart
					(
						  best
						, m_default
						, m_dataset_batch
						, get_random_func()
						, m_target_function
					);
					restart_test_count = 0;
					restart_last_eval = best_eval;
					//restart inc
					++restart_count;
				}
			}
			//output
			if (m_output->is_enable()) m_output->send_best(pass+1, restart_count, double(best_eval), double(best->m_eval));
			//next
			next_batch();
		}
		//end output
		if (m_output->is_enable())
		{
			m_output->end(double(execute_test(*best)));
		} 
		//result
		return best;
	}

	//find best individual (validation test)
	bool find_best(size_t& out_i, ScalarType& out_eval)
	{
		//ref to pop
		auto population = m_population.current();
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
		//ref to pop
		return m_population.current()[best_i];
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
	RandomFunction get_random_func() const
	{
		ScalarType min = m_params.m_range_min;
		ScalarType max = m_params.m_range_max;
		return [=](ScalarType x) -> ScalarType
		{
			return ScalarType(RandomIndices::random(min,max));
		};
	}
	/////////////////////////////////////////////////////////////////
	//eval all
	void serial_execute_target_function_on_all_population()
	{
		//np
		size_t np = (size_t)m_params.m_np;
		//pop ref
		auto& popolation = m_population.current();
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
		auto& popolation = m_population.current();
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
	//serach space
	DBPopulation        m_population;
	PromiseList	        m_promises;
	RuntimeOutput::SPtr m_output;
	CostFunction        m_target_function;
	//dataset
	typename Individual::SPtr  m_default;
	DataSetLoader*		       m_dataset_loader;
	DataSet				       m_dataset_batch;
	//params of DE
	Parameters 				   m_params;
	MutationPtr                m_muation;
	CrossoverPtr               m_crossover;
};

}