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

class DennAlgorithm
{
public:
	////////////////////////////////////////////////////////////////////////
	//Alias
	using LayerList      = typename NeuralNetwork::LayerList;
	//Search space
	using DBPopulation   = DoubleBufferPopulation;
	using IndividualPtr  = typename DoubleBufferPopulation::IndividualPtr;
	using RandomFunction = typename DoubleBufferPopulation::RandomFunction;
	using CostFunction   = typename DoubleBufferPopulation::CostFunction;
	//DE parallel
	using PromiseList    = std::vector< std::future<void> >;
	//Ref mutation crossover
	using MutationPtr    = std::unique_ptr < Mutation >;
	using CrossoverPtr   = std::unique_ptr < Crossover >;
	//Ref mutation
	////////////////////////////////////////////////////////////////////////
	//structs utilities
	struct RestartContext
	{
		Scalar m_last_eval;
		size_t	   m_test_count;
		size_t	   m_count;
        //
		RestartContext(
			  Scalar last_eval  = Scalar(0.0)
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
		Scalar    m_eval;
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
		if (RandomIndices::random() < Scalar(m_params.m_jde_f))   
			i_final.m_f = Scalar(RandomIndices::random(0.0,2.0));
		else														
			i_final.m_f = i_target.m_f;
		//cr JDE
		if (RandomIndices::random() < Scalar(m_params.m_jde_cr))   
			i_final.m_cr = Scalar(RandomIndices::random());
		else														
			i_final.m_cr = i_target.m_cr;

		return true;
	}
	////////////////////////////////////////////////////////////////////////
	DennAlgorithm
	(
		DataSetLoader*      dataset_loader
		, const Parameters&   params
		, const NeuralNetwork nn_default
		, CostFunction		  target_function
		, RuntimeOutput::SPtr output
		, ThreadPool*		  thpool = nullptr
	);
	
	//init
	bool reset();
	
	//big loop
	virtual IndividualPtr execute();

	//find best individual (validation test)
	bool find_best(size_t& out_i, Scalar& out_eval);
	
	//find best individual (validation test)
	IndividualPtr find_best(Scalar& out_eval);

	//using the test set on a individual
	Scalar execute_test(Individual& individual);

protected:
	/////////////////////////////////////////////////////////////////
	//Intermedie steps
	virtual void execute_a_pass(size_t pass, size_t n_sub_pass, BestContext& ctx_best, RestartContext& ctx_restart);
	void execute_a_sub_pass(size_t pass, size_t sub_pass);
	void  execute_update_best(BestContext& ctx_best);
	void  execute_update_restart(size_t pass, const BestContext& ctx_best, RestartContext& ctx);
	/////////////////////////////////////////////////////////////////
	//execute a pass
	void execute_pass();
	void serial_execute_pass();
	void parallel_execute_pass(ThreadPool& thpool);
	void  execute_generation_task(size_t i);
	/////////////////////////////////////////////////////////////////
	//eval all
	void execute_target_function_on_all_population();
	void serial_execute_target_function_on_all_population();
	void parallel_execute_target_function_on_all_population(ThreadPool& thpool);
	/////////////////////////////////////////////////////////////////
	//get random function
	RandomFunction get_random_func() const;
	//load next batch
	bool next_batch();
	/////////////////////////////////////////////////////////////////
	//serach space
	DBPopulation        m_population;
	PromiseList	        m_promises;
	RuntimeOutput::SPtr m_output;
	CostFunction        m_target_function;
	//dataset
	typename Individual::SPtr  m_default;
	DataSetLoader*		       m_dataset_loader;
	DataSetScalar		       m_dataset_batch;
	//threads
	ThreadPool*				   m_thpool;
	//params of DE
	Parameters 				   m_params;
	MutationPtr                m_mutation;
	CrossoverPtr               m_crossover;
};

}