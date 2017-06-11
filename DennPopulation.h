#pragma once
#include "Config.h"
#include "CostFunction.h"
#include "NeuralNetwork.h"

namespace Denn
{
    ////////////////////////////////////////////////////////////////////////
    template < typename Network >
    class Individual : public std::enable_shared_from_this< Individual<Network> >
    {
    public:
        //alias
        using LayerType    = typename Network::LayerType;
        using MatrixType   = typename Network::MatrixType;
        using ScalarType   = typename Network::ScalarType;
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
        //copy attributes from a other individual
        void copy_attributes(const Individual& i)
        {
            m_f    = i.m_f;
            m_cr   = i.m_cr;
            m_eval = i.m_eval;
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

    ////////////////////////////////////////////////////////////////////////
	//Population
    template < typename Network >
	class Population
	{
	public:

		//pointer type		
        using ScalarType    = typename Network::ScalarType;
        using MatrixType    = typename Network::MatrixType;
		using IndividualPtr = typename Individual<Network>::SPtr;

		//vector methods 		
		size_t size() const { return m_individuals.size(); }
		void   resize(size_t i){ m_individuals.resize(i); }
		void   push_back(const IndividualPtr& i){ m_individuals.push_back(i); }

		//vector operator
		      IndividualPtr& operator[](size_t i)       { return m_individuals[i]; }
		const IndividualPtr& operator[](size_t i) const { return m_individuals[i]; }
		
		//iterator
		typename std::vector < IndividualPtr >::iterator       begin()       { return m_individuals.begin(); }
		typename std::vector < IndividualPtr >::const_iterator begin() const { return m_individuals.begin(); }
		typename std::vector < IndividualPtr >::iterator       end()         { return m_individuals.end();   }
		typename std::vector < IndividualPtr >::const_iterator end()   const { return m_individuals.end();   }

		//costum
		void best(size_t& out_i, ScalarType& out_eval) const
		{
			//best
			size_t	   best_i;
			ScalarType best_eval;
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
		IndividualPtr best() const
		{
			//values
			size_t best_i;
			ScalarType best_eval;
			//get best id
			best(best_i, best_eval);
			//return
			return m_individuals[best_i];
		}
	
	protected:

		std::vector < IndividualPtr > m_individuals;

	};

    ////////////////////////////////////////////////////////////////////////
	//Double Population buffer
    template < typename Network, typename DataSet >
	struct DoubleBufferPopulation
	{
        //alias
        using Population     = Denn::Population<Network>;
        using Individual     = Denn::Individual<Network>;
        using IndividualPtr  = typename Denn::Individual<Network>::SPtr;

        using ScalarType     = typename Network::ScalarType;
        using MatrixType     = typename Network::MatrixType;
        using RandomFunction = std::function<ScalarType(ScalarType)>;
		using CostFunction   = std::function<ScalarType(const MatrixType&, const MatrixType&) >;
        //attributes
		size_t     m_current { 0 };
		Population m_pop_buffer[2];
		//init population
		void init(
			  size_t np
			, const IndividualPtr& i_default
			, const DataSet& dataset
			, const RandomFunction random_func
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
				//init
				for (auto& i_individual : population)
				{
					i_individual = i_default->copy();
				}
			}
			//ref to current
			Population& population = current();
			//random init
			for (auto& individual : population)
			for (auto& layer : individual->m_network)
			{
				layer.weights() = layer.weights().unaryExpr(random_func);
				layer.baias() = layer.baias().unaryExpr(random_func);
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
		const Population& const_current() const
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
		const Population& const_next() const
		{
			return m_pop_buffer[(m_current + 1) % 2];
		}
		//get best
		void best(size_t& best_i, ScalarType& out_eval) const
		{
			current().best(best_i,out_eval);
		}
		IndividualPtr best() const
		{
			return current().best();
		}
		//swap
		void swap()
		{
			m_current = (m_current + 1) % 2;
		}
		//restart
		void restart
		(
			  IndividualPtr best
			, const IndividualPtr& i_default
			, const DataSet& dataset
			, const RandomFunction random_func
			, CostFunction target_function
		)
		{
			//ref to current
			Population& population = current();
			//random init
			for (auto& individual : population)
			{
				//Copy default params
				individual->copy_attributes(*i_default);
				//Reinit layers
				for (auto& layer : individual->m_network)
				{
					layer.weights() = layer.weights().unaryExpr(random_func);
					layer.baias()   = layer.baias().unaryExpr(random_func);
				}
			}
			//add best
			size_t rand_i = RandomIndices::irand(population.size());
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
}