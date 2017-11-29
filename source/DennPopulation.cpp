#include "Config.h"
#include "DennPopulation.h"
namespace Denn
{
	////////////////////////////////////////////////////////////////////////
	//Population
	//vector methods 		
	size_t Population::size() const                         { return m_individuals.size(); }
	void   Population::resize(size_t i)                     { m_individuals.resize(i);     }
	void   Population::push_back(const Individual::SPtr& i) { m_individuals.push_back(i);  }
	void   Population::pop_back()                           { m_individuals.pop_back();    }
	void   Population::clear()                              { m_individuals.clear();	   }		

	//vector operator
	Individual::SPtr&       Population::first() { return m_individuals[0];        }
	Individual::SPtr&       Population::last()  { return m_individuals[size()-1]; }
	Individual::SPtr&       Population::operator[](size_t i) { return m_individuals[i]; }
	const Individual::SPtr& Population::operator[](size_t i) const { return m_individuals[i]; }

	//iterator
	typename std::vector < Individual::SPtr >::iterator       Population::begin()       { return m_individuals.begin(); }
	typename std::vector < Individual::SPtr >::const_iterator Population::begin() const { return m_individuals.begin(); }
	typename std::vector < Individual::SPtr >::iterator       Population::end()         { return m_individuals.end(); }
	typename std::vector < Individual::SPtr >::const_iterator Population::end() const   { return m_individuals.end(); }

	//costum
	void Population::best(size_t& out_i, Scalar& out_eval) const
	{
		//best
		size_t	   best_i;
		Scalar best_eval;
		//find best
		for (size_t i = 0; i != m_individuals.size(); ++i)
		{
			//
			if 
			(!i 
			 || ( m_minimize_loss_function && m_individuals[i]->m_eval < best_eval)
			 || (!m_minimize_loss_function && m_individuals[i]->m_eval > best_eval)
			)
			{
				best_i = i;
				best_eval = m_individuals[i]->m_eval;
			}
		}
		out_i = best_i;
		out_eval = best_eval;
	}
	Individual::SPtr Population::best() const
	{
		//values
		size_t best_i;
		Scalar best_eval;
		//get best id
		best(best_i, best_eval);
		//return
		return m_individuals[best_i];
	}

	static bool compare_individual(const Individual::SPtr& li, const Individual::SPtr& ri)
	{
		return li->m_eval < ri->m_eval;
	}

	void Population::sort()
	{
		std::sort(m_individuals.begin(), m_individuals.end(), compare_individual);
	}

	Population Population::copy() const
	{
		//malloc 
		Population new_pop;
		new_pop.resize(size());
		//init
		for(size_t i = 0;i != size(); ++i)
		{
			new_pop[i] = m_individuals[i]->copy();
		}
		//return
		return new_pop;
	}
	////////////////////////////////////////////////////////////////////////
	//init population
	void DoubleBufferPopulation::init(
		  size_t np
		, const Individual::SPtr& i_default
		, const DataSet& dataset
		, const RandomFunction random_func
		, Evaluation& loss_function
		, ThreadPool* thread_pool
	)
	{
		//init
		if(thread_pool)
		{
			//alloc promises
			PromiseList promises;
			//eval
			Population& population = parents();
			//size
			population.resize(np);
			//parallel
			for (size_t i = 0; i != population.size(); ++i)
			{
				promises.push_back(thread_pool->push_task([&,i]()
				{
					//copy layout
					population[i] = i_default->copy();
					//init
					for (auto& layer : population[i]->m_network)
					for (auto& matrix : *layer)
					{
						matrix = matrix.unaryExpr(random_func);
					}
					//eval
					population[i]->m_eval = loss_function(*population[i], dataset);
				}));
			}
			//wait
			for (auto& p : promises) p.wait();
		}
		else 
		{
			for (Population& population : m_pop_buffer)
			{
				//size pop
				population.resize(np);
				//init
				for (Individual::SPtr& individual : population)
				{
					//copy layout
					individual = i_default->copy();
					//init layers
					for (auto& layer : individual->m_network)
					for (auto& matrix : *layer)
					{
						matrix = matrix.unaryExpr(random_func);
					}
				}
			}
			//ref to current
			Population& population = parents();
			//eval
			for (size_t i = 0; i != population.size(); ++i)
			{
				population[i]->m_eval = loss_function(*population[i], dataset);
			}
		}
		//minimize?
		m_minimize_loss_function = loss_function.minimize();
		parents().m_minimize_loss_function = m_minimize_loss_function;
		sons().m_minimize_loss_function = m_minimize_loss_function;
	}
	//size
	size_t DoubleBufferPopulation::size() const
	{
		return m_pop_buffer[0].size();
	}
	void DoubleBufferPopulation::resize(size_t new_np)
	{
		for (Population& population : m_pop_buffer)
		{
			//new size
			population.resize(new_np);
		}
	}
	//current
	Population& DoubleBufferPopulation::parents()
	{
		return m_pop_buffer[size_t(PopulationType::PT_PARENTS)];
	}
	const Population& DoubleBufferPopulation::parents() const
	{
		return m_pop_buffer[size_t(PopulationType::PT_PARENTS)];
	}
	const Population& DoubleBufferPopulation::const_parents() const
	{
		return m_pop_buffer[size_t(PopulationType::PT_PARENTS)];
	}
	//next
	Population& DoubleBufferPopulation::sons()
	{
		return m_pop_buffer[size_t(PopulationType::PT_SONS)];
	}
	const Population& DoubleBufferPopulation::sons() const
	{
		return m_pop_buffer[size_t(PopulationType::PT_SONS)];
	}
	const Population& DoubleBufferPopulation::const_sons() const
	{
		return m_pop_buffer[size_t(PopulationType::PT_SONS)];
	}
	//get best
	void DoubleBufferPopulation::best(size_t& best_i, Scalar& out_eval) const
	{
		parents().best(best_i, out_eval);
	}
	Individual::SPtr DoubleBufferPopulation::best() const
	{
		return parents().best();
	}
	//swap
	void DoubleBufferPopulation::the_best_sons_become_parents()
	{
		//minimize
		if(m_minimize_loss_function)
		{
			for (size_t i = 0; i != parents().size(); ++i)
			{
				if (sons()[i]->m_eval < parents()[i]->m_eval)
				{
					auto individual_tmp = parents()[i];
					parents()[i] = sons()[i];
					sons()[i] = individual_tmp;
				}
			}
		}
		else 
		{
			for (size_t i = 0; i != parents().size(); ++i)
			{
				if (sons()[i]->m_eval > parents()[i]->m_eval)
				{
					auto individual_tmp = parents()[i];
					parents()[i] = sons()[i];
					sons()[i] = individual_tmp;
				}
			}
		}
	}
	void DoubleBufferPopulation::swap(size_t i)
	{
		auto individual_tmp = parents()[i];
		parents()[i] = sons()[i];
		sons()[i] = individual_tmp;
	}
	//restart
	void DoubleBufferPopulation::restart
	(
		  Individual::SPtr 		  best
		, const size_t            where_put_best
		, const Individual::SPtr& i_default
		, const DataSet&          dataset
		, const RandomFunction    random_func
		, Evaluation& 			  loss_function
		, ThreadPool*			  thread_pool
	)
	{
		//init
		if(thread_pool)
		{			
			//alloc promises
			PromiseList promises;
			//eval
			Population& population = parents();
			//random init
			for (size_t i = 0; i != population.size(); ++i)
			{
				promises.push_back(thread_pool->push_task([&,i]()
				{
					//Copy default params
					population[i]->copy_attributes(*i_default);
					//Reinit layers
					for (auto& layer  : population[i]->m_network)
					for (auto& matrix : *layer)
					{
						matrix = matrix.unaryExpr(random_func);
					}
					//eval
					population[i]->m_eval = loss_function(*population[i], dataset);
				}));
			}
			//wait
			for (auto& p : promises) p.wait(); promises.clear();
			//must copy, The Best Individual can't to be changed during the DE process
			population[where_put_best] = best->copy();
			population[where_put_best]->m_eval = loss_function(*population[where_put_best], dataset);
		}
		else 
		{
			//eval
			Population& population = parents();
			//random init
			for (auto& individual : population)
			{
				//Copy default params
				individual->copy_attributes(*i_default);
				//Reinit layers
				for (auto& layer : individual->m_network)
				for (auto& matrix : *layer)
				{
					matrix = matrix.unaryExpr(random_func);
				}
			}
			//must copy, The Best Individual can't to be changed during the DE process
			population[where_put_best] = best->copy();
			//eval
			for (size_t i = 0; i != population.size(); ++i)
			{
				population[i]->m_eval = loss_function(*population[i], dataset);
			}
		}
		//minimize?
		m_minimize_loss_function = loss_function.minimize();
		parents().m_minimize_loss_function = m_minimize_loss_function;
		sons().m_minimize_loss_function = m_minimize_loss_function;
	}
}