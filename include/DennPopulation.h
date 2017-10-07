#pragma once
#include "Config.h"
#include "DennDataset.h"
#include "DennNeuralNetwork.h"
#include "DennIndividual.h"

namespace Denn
{
    ////////////////////////////////////////////////////////////////////////
	//Population
	class Population
	{
	public:
		//vector methods 		
		size_t size() const;
		void   resize(size_t i);
		void   push_back(const Individual::SPtr& i);
		void   pop_back();
		void   clear();

		//vector operator
		Individual::SPtr& first();
		Individual::SPtr& last();
		Individual::SPtr& operator[](size_t i);
		const Individual::SPtr& operator[](size_t i) const;
		
		//iterator
		typename std::vector < Individual::SPtr >::iterator       begin();
		typename std::vector < Individual::SPtr >::const_iterator begin() const;
		typename std::vector < Individual::SPtr >::iterator       end();
		typename std::vector < Individual::SPtr >::const_iterator end() const;

		//costum
		void best(size_t& out_i, Scalar& out_eval) const;
		Individual::SPtr best() const;
		
		//sort
		void sort();

		//copy
		Population copy() const;
	
	protected:

		std::vector < Individual::SPtr > m_individuals;

	};

    ////////////////////////////////////////////////////////////////////////
	enum class PopulationType : size_t
	{
		PT_PARENTS=0,
		PT_SONS,
		PT_SIZE
	};
	//Double Population buffer
	class DoubleBufferPopulation
	{
	public:
        //Pointer
        using RandomFunction = std::function<Scalar(Scalar)>;
		using CostFunction   = std::function<Scalar(const Matrix&, const Matrix&) >;
        //attributes
		Population m_pop_buffer[ size_t(PopulationType::PT_SIZE) ];
		//init population
		void init(
			size_t np
			, const Individual::SPtr& i_default
			, const DataSet& dataset
			, const RandomFunction random_func
			, CostFunction target_function
		);
		//size
		size_t size() const;
		//resize all pops
		void resize(size_t new_np);
		//get
		Population&       operator[] (size_t i)      { return m_pop_buffer[i]; }
		const Population& operator[] (size_t i) const{ return m_pop_buffer[i]; }
		//current
		Population& parents();
		const Population& parents() const;
		const Population& const_parents() const;
		//next
		Population& sons();
		const Population& sons() const;
		const Population& const_sons() const;
		//get best
		void best(size_t& best_i, Scalar& out_eval) const;
		Individual::SPtr best() const;
		//swap
		void the_best_sons_become_parents();
		void swap(size_t i);
		//restart
		void restart
		(
			  Individual::SPtr best
			, const size_t            where_put_best
			, const Individual::SPtr& i_default
			, const DataSet&          dataset
			, const RandomFunction    random_func
			, CostFunction            target_function
		);
	};
}