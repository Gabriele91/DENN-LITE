#include "DennAlgorithm.h"

namespace Denn
{
	DennAlgorithm::DennAlgorithm
	(
		  DataSetLoader*      dataset_loader
		, const Parameters&   params
		, const NeuralNetwork nn_default
		, CostFunction		  target_function
		, RuntimeOutput::SPtr output
		, ThreadPool*		  thpool
	)
	{
		m_dataset_loader = dataset_loader;
		m_default = std::make_shared<Individual>(m_params.m_default_f, m_params.m_default_cr, nn_default);
		m_target_function = target_function;
		m_params = params;
		m_output = output;
		m_thpool = thpool;
		//methods of mutation and crossover
		m_e_method  = EvolutionMethodFactory::create(m_params.m_evolution_type, *this);
		//functions
		m_random_function = gen_random_func();
		m_clamp_function = gen_clamp_func();
		//init all
		start();
	}

	//init
	bool DennAlgorithm::start()
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
				random_function(),
				m_target_function
			);
		}
		//reset method
		m_e_method->start();
		//true
		return success;
	}

	//big loop
	Individual::SPtr DennAlgorithm::execute()
	{
		//global info
		const size_t n_global_pass = ((size_t)m_params.m_generations / (size_t)m_params.m_sub_gens);
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
	bool DennAlgorithm::find_best(size_t& out_i, Scalar& out_eval)
	{
		//ref to pop
		auto population = m_population.parents();
		//validation
		DataSetScalar validation;
		m_dataset_loader->read_validation(validation);
		//best
		Scalar best_eval;
		size_t	   best_i;
		//find best
		for (size_t i = 0; i != population.size(); ++i)
		{
			auto y = population[i]->m_network.apply(validation.m_features);
			Scalar eval = Denn::CostFunction::accuracy(validation.m_labels, y);
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
	Individual::SPtr DennAlgorithm::find_best(Scalar& out_eval)
	{
		//best 
		size_t best_i;
		find_best(best_i, out_eval);
		//return best
		return m_population.parents()[best_i];
	}

	//using the test set on a individual
	Scalar DennAlgorithm::execute_test(Individual& individual)
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
	//Intermedie steps
	void DennAlgorithm::execute_a_pass(size_t pass, size_t n_sub_pass, BestContext& ctx_best, RestartContext& ctx_restart)
	{
		execute_target_function_on_all_population();
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
				,(m_output->is_enable_compute_test()
				  ? double(execute_test(*(ctx_best.m_best)))
				  : double(0.0))
			);
		}
	}
	void DennAlgorithm::execute_a_sub_pass(size_t pass, size_t sub_pass)
	{
		//pass
		execute_pass();
		//output
		if (m_output->is_enable_pass())
		{
			size_t id_best;
			Scalar val_best;
			m_population.parents().best(id_best, val_best);
			m_output->sent_pass(pass, sub_pass, id_best, val_best);
		}
	}
	void DennAlgorithm::execute_update_best(BestContext& ctx_best)
	{
		//find best
		Scalar curr_eval;
		auto curr = find_best(curr_eval);
		//maximize (accuracy)
		if (ctx_best.m_eval < curr_eval)
		{
			//must copy because "restart" 
			//not copy element then 
			//it can change the values of the best individual
			ctx_best.m_best = curr->copy();
			//save eval (on validation) of best
			ctx_best.m_eval = curr_eval;
		}
	}
	void DennAlgorithm::execute_update_restart(size_t pass, const BestContext& ctx_best, RestartContext& ctx)
	{
		//if not enabled then not reset
		if(!*m_params.m_restart_enable) return ;
		//inc count
		if ((ctx_best.m_eval - ctx.m_last_eval) <= (Scalar)m_params.m_restart_delta)
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
		if ((Scalar)m_params.m_restart_count <= ctx.m_test_count)
		{
			m_population.restart
			(
				ctx_best.m_best
				, m_default
				, m_dataset_batch
				, random_function()
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
	void DennAlgorithm::execute_pass()
	{
		m_e_method->start_a_subgen_pass(m_population);
		if (m_thpool) parallel_execute_pass(*m_thpool);
		else          serial_execute_pass();
		m_e_method->end_a_subgen_pass(m_population);
	}
	void DennAlgorithm::serial_execute_pass()
	{
		//for all
		for (size_t i = 0; i != (size_t)m_params.m_np; ++i)
		{
			execute_generation_task(i);
		}
		//swap
		m_population.the_best_sons_become_parents();
	}
	void DennAlgorithm::parallel_execute_pass(ThreadPool& thpool)
	{
		//alloc promises
		m_promises.resize(m_params.m_np);
		//execute
		for (size_t i = 0; i != (size_t)m_params.m_np; ++i)
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
		m_population.the_best_sons_become_parents();
	}
	void DennAlgorithm::execute_generation_task(size_t i)
	{
		//ref to parents
		auto& parents = m_population.parents();
		//ref to sons
		auto& sons = m_population.sons();
		//get temp individual
		auto& new_son = sons[i];
		//Copy default params
		/* new_son->copy_attributes(*m_default); */ 
		//Compute new individual
		m_e_method->create_a_individual(m_population, i, *new_son);
		//eval
		auto y = new_son->m_network.apply(m_dataset_batch.m_features);
		new_son->m_eval = m_target_function(m_dataset_batch.m_labels, y);
	}
	
	/////////////////////////////////////////////////////////////////
	//eval all
	void DennAlgorithm::execute_target_function_on_all_population()
	{
		//eval on batch
		if (m_thpool) parallel_execute_target_function_on_all_population(*m_thpool);
		else 		  serial_execute_target_function_on_all_population();
	}
	void DennAlgorithm::serial_execute_target_function_on_all_population()
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
			if (std::isnan(i_target.m_eval))
				i_target.m_eval = std::numeric_limits<Scalar>::max();
		}
	}
	void DennAlgorithm::parallel_execute_target_function_on_all_population(ThreadPool& thpool)
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
		return [=](Scalar x) -> Scalar
		{
			return Scalar(Random::uniform(min, max));
		};
	}
	//gen clamp function	
	DennAlgorithm::ClampFunction DennAlgorithm::gen_clamp_func() const
	{
		Scalar min = m_params.m_clamp_min;
		Scalar max = m_params.m_clamp_max;
		return [=](Scalar x) -> Scalar
		{
			return Denn::clamp<Scalar>(x, min, max);
		};
	}
	//load next batch
	bool DennAlgorithm::next_batch()
	{
		return m_dataset_loader->read_batch(m_dataset_batch);
	}
	/////////////////////////////////////////////////////////////////

}