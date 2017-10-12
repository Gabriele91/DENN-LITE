#include "DennAlgorithm.h"

namespace Denn
{
	DennAlgorithm::DennAlgorithm
	(
		  DataSetLoader*      dataset_loader
		, const Parameters&   params
		, const NeuralNetwork nn_default
		, CostFunction		  target_function
		, std::ostream&       output
		, ThreadPool*		  thpool
	)
	: m_main_random(*params.m_seed)
    , m_thpool(thpool)
	, m_target_function(target_function)
	, m_output(RuntimeOutputFactory::create(*params.m_runtime_output_type, output, *this))
	, m_default(std::make_shared<Individual>
	    ( *params.m_default_f
		, *params.m_default_cr
	    , *params.m_perc_of_best
		, nn_default
		))
	, m_dataset_loader(dataset_loader)
	, m_dataset_batch(dataset_loader)
	, m_params(params)
	{
	}

	//init
	bool DennAlgorithm::init()
	{
		//success flag
		bool success = m_dataset_loader != nullptr;
		//init test set
		if(*m_params.m_batch_offset <= 0)
			m_dataset_batch.start_read_batch(*m_params.m_batch_size, *m_params.m_batch_size);
		else
			m_dataset_batch.start_read_batch(*m_params.m_batch_size, *m_params.m_batch_offset);
		//init random engine
		m_main_random.reinit(*m_params.m_seed);
		//gen random function
		m_random_function = gen_random_func();
		//gen clamp functions
		m_clamp_function = gen_clamp_func();
		//clear random engines
		m_population_random.clear();
		//true
		return success;
	}

	bool DennAlgorithm::init_population()
	{
		//get np
		size_t np = (size_t)m_params.m_np; //init current_np();
		//min size
		if (!np) return false;
		//init random engines
		for(size_t i=0; i != np ;++i)
		{
			m_population_random.emplace_back(main_random().uirand());
		}
		//init pop
		m_population.init
		(
			np,
			m_default,
			current_batch(),
			m_random_function,
			m_target_function
		);
		//method of evoluction
		m_e_method = EvolutionMethodFactory::create(m_params.m_evolution_type, *this);
		//reset method
		m_e_method->start();
		//true
		return true;
	}

	//big loop
	Individual::SPtr DennAlgorithm::execute()
	{
		//init all
		if (!init()) 			return nullptr;
		if (!init_population()) return nullptr;
		//global info
		const size_t n_global_pass = ((size_t)m_params.m_generations / (size_t)m_params.m_sub_gens);
		const size_t n_sub_pass = m_params.m_sub_gens;
		//restart init
		m_restart_ctx = RestartContext();
		//best
		if(m_e_method->best_from_validation())
		{
			//maximize
			m_best_ctx = BestContext(nullptr, -std::numeric_limits<Scalar>::max());
		}
		else 
		{
			//minimize
			m_best_ctx = BestContext(nullptr, std::numeric_limits<Scalar>::max());				
		}
		execute_update_best();
		//start output
		if (m_output) m_output->start();
		//main loop
		for (size_t pass = 0; pass != n_global_pass; ++pass)
		{
			execute_a_pass(pass, n_sub_pass);
			//next
			next_batch();
		}
		//end output
		if (m_output) m_output->end();
		//result
		return m_best_ctx.m_best;
	}

	//using the test set on a individual
	Scalar DennAlgorithm::execute_test() const 
	{
		//validation
		DataSetScalar test;
		m_dataset_loader->read_test(test);
		//compute
		auto y = m_best_ctx.m_best->m_network.apply(test.m_features);
		Scalar eval = Denn::CostFunction::accuracy(test.m_labels, y);
		//return
		return eval;
	}
	Scalar DennAlgorithm::execute_test(Individual& individual) const 
	{
		//validation
		DataSetScalar test;
		m_dataset_loader->read_test(test);
		//compute
		auto y = individual.m_network.apply(test.m_features);
		Scalar eval = Denn::CostFunction::accuracy(test.m_labels, y);
		//return
		return eval;
	}
	/////////////////////////////////////////////////////////////////
	//test
	//find best individual (validation test)
	Individual::SPtr DennAlgorithm::find_best_on_validation(Scalar& out_eval)
	{
		//best 
		size_t best_i;
		find_best_on_validation(best_i, out_eval);
		//return best
		return m_population.parents()[best_i];
	}
	bool DennAlgorithm::find_best_on_validation(size_t& out_i, Scalar& out_eval)
	{
		if(m_thpool) return parallel_find_best_on_validation(*m_thpool,out_i,out_eval);
		else 		 return serial_find_best_on_validation(out_i, out_eval);
	}
	bool DennAlgorithm::serial_find_best_on_validation(size_t& out_i, Scalar& out_eval)
	{
		//ref to pop
		auto& population = m_population.parents();
		//validation
		DataSetScalar validation;
		m_dataset_loader->read_validation(validation);
		//best
		Scalar best_eval = std::numeric_limits<Scalar>::min();
		size_t	   best_i= 0;
		//find best
		for (size_t i = 0; i != current_np(); ++i)
		{
			auto y = population[i]->m_network.apply(validation.m_features);
			Scalar eval = Denn::CostFunction::accuracy(validation.m_labels, y);
			//safe
			if (std::isnan(eval)) eval = std::numeric_limits<Scalar>::min();
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
	bool DennAlgorithm::parallel_find_best_on_validation(ThreadPool& thpool, size_t& out_i, Scalar& out_eval)
	{
		//ref to pop
		auto& population = m_population.parents();
		//get np
		size_t np = current_np();
		//validation
		DataSetScalar validation;
		m_dataset_loader->read_validation(validation);
		//list eval
		std::vector<Scalar> validation_evals(population.size(), std::numeric_limits<Scalar>::min());
		//alloc promises
		m_promises.resize(np);
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			//ref to target
			auto& i_target = *population[i];
			auto& eval     = validation_evals[i];
			//add
			m_promises[i] = thpool.push_task([&]()
			{
				//test
				auto y = i_target.m_network.apply(validation.m_features);
				eval = Denn::CostFunction::accuracy(validation.m_labels, y);
				//safe
				if (std::isnan(eval)) eval = std::numeric_limits<Scalar>::min();
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
		//find best
		//maximize (accuracy)
		out_i    = std::distance(validation_evals.begin(), std::max_element(validation_evals.begin(), validation_evals.end()));
		out_eval = validation_evals[out_i];
		return true;

	}
	
	/////////////////////////////////////////////////////////////////
	//Intermedie steps
	void DennAlgorithm::execute_a_pass(size_t pass, size_t n_sub_pass)
	{
		execute_target_function_on_all_population(m_population.parents());
		///////////////////////////////////////////////////////////////////
		//output
		if(m_output) m_output->start_a_pass();
		//start pass
		m_e_method->start_a_gen_pass(m_population);
		//sub pass
		for (size_t sub_pass = 0; sub_pass != n_sub_pass; ++sub_pass)
		{
			execute_a_sub_pass(pass, sub_pass);
		}
		//end pass
		m_e_method->end_a_gen_pass(m_population);
		//update context
		execute_update_best();
		//restart
		if(m_e_method->can_reset()) execute_update_restart(pass);
		//output
		if(m_output) m_output->end_a_pass();
	}
	void DennAlgorithm::execute_a_sub_pass(size_t pass, size_t sub_pass)
	{
		//output
		if(m_output) m_output->start_a_sub_pass();
		//pass
		execute_pass();
		//output
		if(m_output) m_output->end_a_sub_pass();
	}
	void DennAlgorithm::execute_update_best()
	{
		if(m_e_method->best_from_validation()) execute_update_best_on_validation();
		else             					   execute_update_best_on_target_function();
	}	
	void DennAlgorithm::execute_update_best_on_validation()
	{
		//find best
		Scalar curr_eval = Scalar(0.0);
		auto curr = find_best_on_validation(curr_eval);
		//maximize (accuracy)
		if (m_best_ctx.m_eval < curr_eval)
		{
			//must copy because "restart" 
			//not copy element then 
			//it can change the values of the best individual
			m_best_ctx.m_best = curr->copy();
			//save eval (on validation) of best
			m_best_ctx.m_eval = curr_eval;
		}
	}
	void DennAlgorithm::execute_update_best_on_target_function()
	{
		//find best
		auto curr = m_population.parents().best();
		//minimize (target function)
		if (curr->m_eval < m_best_ctx.m_eval)
		{
			//must copy because "restart" 
			//not copy element then 
			//it can change the values of the best individual
			m_best_ctx.m_best = curr->copy();
			//save eval (on validation) of best
			m_best_ctx.m_eval = curr->m_eval;
		}
	}
	void DennAlgorithm::execute_update_restart(size_t pass)
	{
		//if not enabled then not reset
		if(!*m_params.m_restart_enable) return ;
		//inc count
		if ((m_best_ctx.m_eval - m_restart_ctx.m_last_eval) <= (Scalar)m_params.m_restart_delta)
		{
			++m_restart_ctx.m_test_count;
		}
		else
		{
			m_restart_ctx.m_test_count = 0;
			m_restart_ctx.m_last_eval = m_best_ctx.m_eval;
		}
		//first
		if (!pass) m_restart_ctx.m_last_eval = m_best_ctx.m_eval;
		//test
		if ((Scalar)m_params.m_restart_count <= m_restart_ctx.m_test_count)
		{
			m_population.restart
			(
				  m_best_ctx.m_best					     //best
				, main_random().index_rand(current_np()) //where put
				, m_default							     //default individual
				, current_batch()						 //current batch
				, m_random_function				         //random generator
				, m_target_function				         //fitness function	  
			);
			m_restart_ctx.m_test_count = 0;
			m_restart_ctx.m_last_eval = m_best_ctx.m_eval;
			//restart inc
			++m_restart_ctx.m_count;
			//output
			if(m_output) m_output->restart();
		}
	}
	
	/////////////////////////////////////////////////////////////////
	//execute a pass
	void DennAlgorithm::execute_pass()
	{
		m_e_method->start_a_subgen_pass(m_population);
		if (m_thpool) parallel_execute_pass(*m_thpool);
		else          serial_execute_pass();
		m_e_method->end_a_subgen_pass(m_population);
	}
	void DennAlgorithm::serial_execute_pass()
	{
		//get np
		size_t np = current_np();
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			execute_generation_task(i);
		}
		//swap
		m_e_method->selection(m_population);
	}
	void DennAlgorithm::parallel_execute_pass(ThreadPool& thpool)
	{
		//get np
		size_t np = current_np();
		//alloc promises
		m_promises.resize(np);
		//execute
		for (size_t i = 0; i != np; ++i)
		{
			//add
			m_promises[i] = thpool.push_task([this, i]()
			{
				execute_generation_task(i);
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
		//swap
		m_e_method->selection(m_population);
	}
	void DennAlgorithm::execute_generation_task(size_t i)
	{
		//ref to sons
		auto& sons = m_population.sons();
		//get temp individual
		auto& new_son = sons[i];
		//Compute new individual
		m_e_method->create_a_individual(m_population, i, *new_son);
		//eval
		auto y = new_son->m_network.apply(current_batch().m_features);
		new_son->m_eval = m_target_function(current_batch().m_labels, y);
	}
	
	/////////////////////////////////////////////////////////////////
	//fitness function on a population
	void DennAlgorithm::execute_fitness_on(Population& population) const
	{
		execute_target_function_on_all_population(population);
	}
	void DennAlgorithm::execute_target_function_on_all_population(Population& population) const
	{
		//eval on batch
		if (m_thpool) parallel_execute_target_function_on_all_population(population, *m_thpool);
		else 		  serial_execute_target_function_on_all_population(population);
	}
	void DennAlgorithm::serial_execute_target_function_on_all_population(Population& population) const
	{
		//np
		size_t np = current_np();
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			//ref to target
			auto& i_target = *population[i];
			//eval
			auto y = i_target.m_network.apply(current_batch().m_features);
			i_target.m_eval = m_target_function(current_batch().m_labels, y);
			//safe
			if (std::isnan(i_target.m_eval))
				i_target.m_eval = std::numeric_limits<Scalar>::max();
		}
	}
	void DennAlgorithm::parallel_execute_target_function_on_all_population(Population& population, ThreadPool& thpool) const
	{
		//get np
		size_t np = current_np();
		//alloc promises
		m_promises.resize(np);
		//for all
		for (size_t i = 0; i != np; ++i)
		{
			//ref to target
			auto& i_target = *population[i];
			//add
			m_promises[i] = thpool.push_task([this,&i_target]()
			{
				//test
				auto y = i_target.m_network.apply(current_batch().m_features);
				i_target.m_eval = m_target_function(current_batch().m_labels, y);
				//safe
				if (std::isnan(i_target.m_eval))
					i_target.m_eval = std::numeric_limits<Scalar>::max();
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
	}
	
	/////////////////////////////////////////////////////////////////
	//gen random function
	DennAlgorithm::RandomFunction DennAlgorithm::gen_random_func() const
	{
		Scalar min = m_params.m_range_min;
		Scalar max = m_params.m_range_max;
		return [this,min,max](Scalar x) -> Scalar
		{
			return Scalar(main_random().uniform(min, max));
		};
	}
	//gen clamp function	
	DennAlgorithm::ClampFunction DennAlgorithm::gen_clamp_func() const
	{
		Scalar min = m_params.m_clamp_min;
		Scalar max = m_params.m_clamp_max;
		return [min,max](Scalar x) -> Scalar
		{
			return Denn::clamp<Scalar>(x, min, max);
		};
	}
	//load next batch
	bool DennAlgorithm::next_batch()
	{
		m_dataset_batch.read_batch();
		return true;
	}
	/////////////////////////////////////////////////////////////////

}
