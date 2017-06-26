#include "DennPopulation.h"
namespace Denn
{
	////////////////////////////////////////////////////////////////////////
	//Population
	//vector methods 		
	size_t Population::size() const { return m_individuals.size(); }
	void   Population::resize(size_t i) { m_individuals.resize(i); }
	void   Population::push_back(const IndividualPtr& i) { m_individuals.push_back(i); }

	//vector operator
	Population::IndividualPtr&       Population::operator[](size_t i) { return m_individuals[i]; }
	const Population::IndividualPtr& Population::operator[](size_t i) const { return m_individuals[i]; }

	//iterator
	typename std::vector < Population::IndividualPtr >::iterator       Population::begin() { return m_individuals.begin(); }
	typename std::vector < Population::IndividualPtr >::const_iterator Population::begin() const { return m_individuals.begin(); }
	typename std::vector < Population::IndividualPtr >::iterator       Population::end() { return m_individuals.end(); }
	typename std::vector < Population::IndividualPtr >::const_iterator Population::end() const { return m_individuals.end(); }

	//costum
	void Population::best(size_t& out_i, Scalar& out_eval) const
	{
		//best
		size_t	   best_i;
		Scalar best_eval;
		//find best
		for (size_t i = 0; i != m_individuals.size(); ++i)
		{
			//minimize (cross_entropy)
			if (!i || m_individuals[i]->m_eval < best_eval)
			{
				best_i = i;
				best_eval = m_individuals[i]->m_eval;
			}
		}
		out_i = best_i;
		out_eval = best_eval;
	}
	Population::IndividualPtr Population::best() const
	{
		//values
		size_t best_i;
		Scalar best_eval;
		//get best id
		best(best_i, best_eval);
		//return
		return m_individuals[best_i];
	}
	////////////////////////////////////////////////////////////////////////
	//init population
	void DoubleBufferPopulation::init(
		size_t np
		, const IndividualPtr& i_default
		, const DataSet& dataset
		, const RandomFunction random_func
		, CostFunction target_function
	)
	{
		//init pop
		for (Population& population : m_pop_buffer)
		{
			//size
			population.resize(np);
			//init
			for (auto& i_individual : population)
			{
				i_individual = i_default->copy();
			}
		}
		//ref to current
		Population& population = parents();
		//random init
		for (auto& individual : population)
			for (auto& layer : individual->m_network)
				for (auto& matrix : *layer)
				{
					matrix = matrix.unaryExpr(random_func);
				}
		//eval
		for (size_t i = 0; i != population.size(); ++i)
		{
			auto y = population[i]->m_network.apply(dataset.features());
			population[i]->m_eval = target_function(dataset.labels(), y);
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
	DoubleBufferPopulation::IndividualPtr DoubleBufferPopulation::best() const
	{
		return parents().best();
	}
	//swap
	void DoubleBufferPopulation::the_best_sons_become_parents()
	{
		//minimize (cross_entropy)
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
	//restart
	void DoubleBufferPopulation::restart
	(
		  IndividualPtr best
		, const IndividualPtr& i_default
		, const DataSet& dataset
		, const RandomFunction random_func
		, CostFunction target_function
	)
	{
		//ref to current
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
		//add best
		size_t rand_i = RandomIndices::irand(population.size());
		//must copy, The Best Individual can't to be changed during the DE process
		population[rand_i] = best->copy();
		//eval
		for (size_t i = 0; i != population.size(); ++i)
		{
			auto y = population[i]->m_network.apply(dataset.features());
			population[i]->m_eval = target_function(dataset.labels(), y);
		}
	}
}