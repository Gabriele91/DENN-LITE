#pragma once
#include "Config.h"
#include "ThreadPool.h"
#include "CostFunction.h"
#include "RandomIndices.h"
#include "DatasetLoader.h"
#include "NeuralNetwork.h"

namespace Denn
{

class PopulationRandomIndexGenerator
{
public:

	PopulationRandomIndexGenerator(size_t np, size_t n_index = 3)
	{
		m_np = np;
		m_randoms_i.resize(n_index);
	}

	void DoRand(int individual)
	{
		RandomIndices::n_rand_different_indices(m_np, individual, m_randoms_i, m_randoms_i.size());
	}

	int operator[](size_t i) const
	{
		return m_randoms_i[i];
	}

protected:

	size_t				m_np;
	std::vector < int > m_randoms_i;

};

class RuntimeOutput : public std::enable_shared_from_this< RuntimeOutput >
{
public:
	using SPtr = std::shared_ptr<RuntimeOutput>;

	RuntimeOutput() {}

	SPtr get_ptr(){ return shared_from_this(); }

	virtual bool is_enable()	   { return true;     }
	virtual std::ostream& output() { return std::cerr; }


};

#define DENN_RUNTIME_OUTPUT(x,y) if((x)->is_enable()){ (x)->output() << y << std::endl; }

template< typename Network, typename DataSetLoader >
class DennAlgorithm
{
public:
	////////////////////////////////////////////////////////////////////////
	using LayerType    = typename Network::LayerType;
	using MatrixType   = typename Network::MatrixType;
	using ScalarType   = typename Network::ScalarType;
	using LayerList    = typename Network::LayerList;
	using DataSet	   = DataSetRaw< ScalarType >;
	using CostFunction = std::function < ScalarType (const MatrixType&, const MatrixType&) >;
	using PromiseList  = std::vector< std::future<void> >;
	////////////////////////////////////////////////////////////////////////
	struct FCrInfo
	{
		ScalarType m_f{ ScalarType(1.0) };
		ScalarType m_cr{ ScalarType(1.0) };

		FCrInfo() {}
		FCrInfo(ScalarType f, ScalarType cr) :m_f(f), m_cr(cr) {}
	};
	struct JDEFCrInfo
	{
		ScalarType m_f{ ScalarType(0.1) };
		ScalarType m_cr{ ScalarType(0.1) };

		JDEFCrInfo() {}
		JDEFCrInfo(ScalarType f, ScalarType cr) :m_f(f), m_cr(cr) {}
	};
	struct ClampInfo
	{
		ScalarType m_min{ ScalarType(-1.0) };
		ScalarType m_max{ ScalarType(1.0) };

		ClampInfo() {}
		ClampInfo(ScalarType min, ScalarType max) :m_min(min), m_max(max) {}
	};
	struct RestartInfo
	{
		bool		m_enable{ false };
		size_t		m_count_fail{ 0 };
		ScalarType  m_delta{ ScalarType(0.01) };

		RestartInfo() {}
		RestartInfo(size_t count, ScalarType delta = 0.01) :m_enable(true), m_count_fail(count), m_delta(delta) {}
		RestartInfo(bool enable, size_t count, ScalarType delta = 0.01) :m_enable(enable), m_count_fail(count), m_delta(delta) {}
	};
	struct RandomRangeInfo
	{
		ScalarType m_min{ ScalarType(-1.0) };
		ScalarType m_max{ ScalarType(1.0) };

		RandomRangeInfo() {}
		RandomRangeInfo(ScalarType min, ScalarType max) :m_min(min), m_max(max) {}

		ScalarType range() const
		{
			return m_max - m_min;
		}

		std::function<ScalarType(ScalarType)> get_unary_expr() const
		{
			return [this](ScalarType x) -> ScalarType
			{
				return (RandomIndices::random_0_to_1() * range()) + m_min;
			};
		}
	};
	////////////////////////////////////////////////////////////////////////
	class Individual : public std::enable_shared_from_this< Individual >
	{
	public:
		//ref to individual
		using SPtr = std::shared_ptr<Individual>;
		//return ptr
		SPtr get_ptr(){ return this->shared_from_this(); }
		//shared copy
		SPtr copy() const
		{
			return std::make_shared<Individual>(*this);
		}
		//attributes
		ScalarType m_eval{ std::numeric_limits<ScalarType>::max() };
		ScalarType m_f   { 1.0 };
		ScalarType m_cr  { 1.0 };
		Network	   m_network;
		//init
		Individual() {}
		Individual(ScalarType f, ScalarType cr, const Network& network)
		{
			m_f		  = f;
			m_cr	  = cr;
			m_network = network;
		}
		//cast
		operator Network&()
		{
			return m_network;
		}

		operator const Network& () const
		{
			return m_network;
		}
		//like Network
		LayerType& operator[](size_t i)
		{
			return m_network[i];
		}
		const LayerType& operator[](size_t i) const
		{
			return m_network[i];
		}
		size_t size() const
		{
			return m_network.size();
		}
	};
	//types
	using Population = std::vector < typename Individual::SPtr >;
	//population pass
	struct DoubleBufferPopulation
	{
		size_t     m_current { 0 };
		Population m_pop_buffer[2];
		//init population
		void init(
			  size_t np
			, const typename Individual::SPtr& i_default
			, const DataSet& dataset
			, const RandomRangeInfo& random_range_info
			, CostFunction target_function
		)
		{
			//init counter
			m_current = 0;
			//init pop
			for (Population& population : m_pop_buffer)
			{
				//size
				population.resize(np);
			}
			//ref to current
			Population& population = current();
			//init
			for (typename Individual::SPtr& i_individual : population)
			{
				i_individual = i_default->copy();
			}
			//random exp
			auto random_exp = random_range_info.get_unary_expr();
			//random init
			for (typename Individual::SPtr& individual : population)
			for (LayerType& layer : individual->m_network)
			{
				layer.weights() = layer.weights().unaryExpr(random_exp);
				layer.baias() = layer.baias().unaryExpr(random_exp);
			}
			//eval
			for (size_t i = 0; i != population.size(); ++i)
			{
				auto y = population[i]->m_network.apply(dataset.m_features);
				population[i]->m_eval = target_function(dataset.m_labels, y);
			}
		}
		//current
		Population& current()
		{
			return m_pop_buffer[m_current];
		}
		const Population& current() const
		{
			return m_pop_buffer[m_current];
		}
		//next
		Population& next()
		{
			return m_pop_buffer[(m_current+1)%2];
		}
		const Population& next() const
		{
			return m_pop_buffer[(m_current + 1) % 2];
		}
		//swap
		void swap()
		{
			m_current = (m_current + 1) % 2;
		}
		//restart
		void restart
		(
			  typename Individual::SPtr best
			, const typename Individual::SPtr& i_default
			, const DataSet& dataset
			, const RandomRangeInfo& random_range_info
			, CostFunction target_function
		)
		{
			//ref to current
			Population& population = current();
			//random exp
			auto random_exp = random_range_info.get_unary_expr();
			//random init
			for (typename Individual::SPtr& individual : population)
			{
				//Copy default params
				individual->m_f   =i_default->m_f;
				individual->m_cr  =i_default->m_cr;
				individual->m_eval=i_default->m_eval;
				//Reinit layers
				for (LayerType& layer : individual->m_network)
				{
					layer.weights() = layer.weights().unaryExpr(random_exp);
					layer.baias()   = layer.baias().unaryExpr(random_exp);
				}
			}
			//add best
			size_t rand_i = RandomIndices::irand(int(population.size()));
			//must copy, The Best Individual can't to be changed during the DE process
			population[rand_i] = best->copy();
			//eval
			for (size_t i = 0; i != population.size(); ++i)
			{
				auto y = population[i]->m_network.apply(dataset.m_features);
				population[i]->m_eval = target_function(dataset.m_labels, y);
			}
		}
	};
	////////////////////////////////////////////////////////////////////////
	DennAlgorithm
	(
		  DataSetLoader* dataset_loader
		, size_t np
		, const Network   nn_default
		, JDEFCrInfo      f_cr_jde			 = JDEFCrInfo()
		, FCrInfo         f_cr_default		 = FCrInfo()
		, ClampInfo       clamp_info		 = ClampInfo()
		, RestartInfo     restart_info		 = RestartInfo()
		, RandomRangeInfo random_range_info  = RandomRangeInfo()
		, CostFunction    target_function	 = Denn::CostFunction::cross_entropy<MatrixType>
		, RuntimeOutput::SPtr      output    = std::make_shared<RuntimeOutput>()
	) 
	{
		m_dataset_loader    = dataset_loader;
		m_np			    = np;
		m_default		    = std::make_shared<Individual>(f_cr_default.m_f, f_cr_default.m_cr, nn_default );
		m_f_cr_jde_info     = f_cr_jde;
		m_clamp_info	    = clamp_info;
		m_restart_info      = restart_info;
		m_random_range_info = random_range_info;
		m_target_function   = target_function;
		m_output			= output;
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
			m_population.init(m_np, m_default, m_dataset_batch, m_random_range_info, m_target_function);
		}
		//true
		return success;
	}
	
	//execute a pass
	void serial_pass()
	{
		//ref to current
		Population& population = m_population.current();
		//ref to next
		Population& population_next = m_population.next();
		//for all
		for(size_t i = 0; i!= population.size(); ++i)
		{
			auto new_individual = m_default->copy();
			//compute
			jde(i, *new_individual);
			rand_one_bin(i, *new_individual);
			//eval
			auto y                 = new_individual->m_network.apply(m_dataset_batch.m_features);
			new_individual->m_eval = m_target_function(m_dataset_batch.m_labels, y);
			//minimixe (cross_entropy)
			if (new_individual->m_eval < population[i]->m_eval)
			{
				population_next[i] = new_individual;
			}
			else
			{
				population_next[i] = population[i];
			}
		}
		//swap
		m_population.swap();
	}

	//execute a pass
	void parallel_pass(ThreadPool& thpool)
	{
		//ref to current
		Population& population      = m_population.current();
		//ref to next
		Population& population_next = m_population.next();
		//alloc promises
		m_promises.resize(population.size());
		//execute
		for (size_t i = 0; i != population.size(); ++i)
		{
			//add
			m_promises[i] = thpool.push_task([this,i,&population,&population_next]()
			{ 
				auto new_individual = m_default->copy();
				//compute
				jde(i, *new_individual);
				rand_one_bin(i, *new_individual);
				//eval
				auto y                 = new_individual->m_network.apply(m_dataset_batch.m_features);
				new_individual->m_eval = m_target_function(m_dataset_batch.m_labels, y);
				//minimixe (cross_entropy)
				if (new_individual->m_eval < population[i]->m_eval)
				{
					population_next[i] = new_individual;
				}
				else
				{
					population_next[i] = population[i];
				}
			});
		}
		//wait
		for (auto& promise : m_promises) promise.wait();
		//swap
		m_population.swap();
	}

	//load next batch
	bool next_batch()
	{
		return m_dataset_loader->read_batch(m_dataset_batch);
	}

	//big loop
	typename Individual::SPtr execute(const size_t n_pass, const size_t n_sub_pass, ThreadPool* thpool = nullptr)
	{
		const size_t n_global_pass = (n_pass / n_sub_pass);
		//restart init
		ScalarType restart_last_eval = 0;
		size_t	   restart_count = 0;
		//best
		typename Individual::SPtr best = m_default->copy();
		ScalarType best_eval  = 0;
		//main loop
		for (size_t pass = 0; pass != n_global_pass; ++pass)
		{
			for (size_t sub_pass = 0; sub_pass != n_sub_pass; ++sub_pass)
			{
				if (thpool)	parallel_pass(*thpool); else serial_pass();
				//output
				DENN_RUNTIME_OUTPUT(m_output, 
				"pass: "       << pass <<
				", sub_pass: " << sub_pass <<
				" -> "         << (n_sub_pass * pass + sub_pass) 
				)

			}
			//find best
			ScalarType curr_eval;
			typename Individual::SPtr curr = find_best(curr_eval);
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
			if (m_restart_info.m_enable)
			{
				//first
				if (!pass) restart_last_eval = best_eval;
				//inc count
				if ((best_eval - restart_last_eval) < m_restart_info.m_delta)
				{
					++restart_count;
				}
				else
				{
					restart_count = 0;
					restart_last_eval = best_eval;
				}
				//test
				if (m_restart_info.m_count_fail <= restart_count)
				{
					m_population.restart
					(
						  best
						, m_default
						, m_dataset_batch
						, m_random_range_info
						, m_target_function
					);
					restart_count = 0;
					restart_last_eval = best_eval;
				}
			}
			//next
			next_batch();
		}
		//result
		return best;
	}

	//find best individual (validation test)
	bool find_best(size_t& out_i, ScalarType& out_eval)
	{
		//ref to pop
		Population& population = m_population.current();
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
	typename Individual::SPtr find_best(ScalarType& out_eval)
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
	ScalarType f_clamp(ScalarType value) const
	{
		return clamp(value, m_clamp_info.m_min, m_clamp_info.m_max);
	}
	/////////////////////////////////////////////////////////////////
	bool jde(int target, Individual& i_final) const
	{
		//vectors
		const Population& population = m_population.current();
		const Individual& i_target   = *population[target];
		//f JDE
		if (RandomIndices::random_0_to_1() < m_f_cr_jde_info.m_f)   
			i_final.m_f = ScalarType(RandomIndices::random_0_to_1() * 2.0);
		else														
			i_final.m_f = i_target.m_f;
		//cr JDE
		if (RandomIndices::random_0_to_1() < m_f_cr_jde_info.m_cr)   
			i_final.m_cr = ScalarType(RandomIndices::random_0_to_1());
		else														
			i_final.m_cr = i_target.m_cr;

		return true;
	}
	bool rand_one_bin(int target, Individual& i_final) const
	{
		//vectors
		const Population& population = m_population.current();
		const Individual& i_target   = *population[target];
		//alias
		const auto& f  = i_final.m_f;
		const auto& cr = i_final.m_cr;
		//init generator
		PopulationRandomIndexGenerator generator(m_np);
		//for each layers
		for (size_t i_layer=0; i_layer != i_target.size(); ++i_layer)
		{
			//weights and baias
			for (size_t m = 0; m!= i_final[i_layer].size(); ++m)
			{
				//do rand
				generator.DoRand(target);
				//do cross + mutation
				const Individual& nn_a = *population[generator[0]];
				const Individual& nn_b = *population[generator[1]];
				const Individual& nn_c = *population[generator[2]];
				//
				auto w_target = i_target[i_layer][m];
				auto w_final  = i_final[i_layer][m];
				auto w_lr_a   = nn_a[i_layer][m];
				auto w_lr_b   = nn_b[i_layer][m];
				auto w_lr_c   = nn_c[i_layer][m];
				//random i
				int rand = RandomIndices::irand(w_lr_a.size());
				//CROSS
				for (size_t e = 0; e != w_lr_a.size(); ++e)
				{
					//cross event
					bool cross_event = RandomIndices::random_0_to_1() < i_target.m_cr;
					//mutation
					if (cross_event || rand == e)
					{
						w_final(e) = f_clamp((w_lr_a(e) - w_lr_b(e)) * f + w_lr_c(e));
					}
					else
					{
						w_final(e) = w_target(e);
					}
				}
			}

		}
		return true;
	}
	/////////////////////////////////////////////////////////////////
	DoubleBufferPopulation  m_population;
	PromiseList			    m_promises;
	//info
	typename Individual::SPtr m_default;
	DataSetLoader*		      m_dataset_loader;
	DataSet				      m_dataset_batch;
	size_t					  m_np;
	JDEFCrInfo			      m_f_cr_jde_info;
	ClampInfo			      m_clamp_info;
	RestartInfo			      m_restart_info;
	RandomRangeInfo		      m_random_range_info;
	CostFunction			  m_target_function;
	RuntimeOutput::SPtr       m_output;
};

}