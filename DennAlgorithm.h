#pragma once
#include "Config.h"
#include "ThreadPool.h"
#include "CostFunction.h"
#include "DennRandom.h"
#include "DennDatasetLoader.h"
#include "DennParameters.h"
#include "DennPopulation.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennEvolutionMethod.h"
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
	using EvolutionMethodPtr = typename EvolutionMethod::SPtr;
	using MutationPtr        = typename Mutation::SPtr;
	using CrossoverPtr       = typename Crossover::SPtr;
	using ClampFunction      = std::function<Scalar(Scalar)>;
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

	//info
	const Parameters& parameters() const
	{
		return m_params;
	}

	const EvolutionMethod& evolution_method() const
	{
		return *m_e_method;
	}

	const Mutation& mutation() const
	{
		return *m_mutation;
	}

	const Crossover& crossover() const
	{
		return *m_crossover;
	}

	const RandomFunction& random_function() const
	{
		return m_random_function;
	}	

	const RandomFunction& clamp_function() const
	{
		return m_clamp_function;
	}

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
	//gen random function
	RandomFunction gen_random_func() const;
	//gen clamp function
	ClampFunction gen_clamp_func() const;
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
	EvolutionMethodPtr         m_e_method;
	MutationPtr                m_mutation;
	CrossoverPtr               m_crossover;
	//function for DE
	RandomFunction			   m_random_function;
	ClampFunction			   m_clamp_function;
};

}