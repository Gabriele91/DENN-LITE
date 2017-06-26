#pragma once
#include "Config.h"
#include "CostFunction.h"
#include "DennDataSet.h"
#include "NeuralNetwork.h"
#include "DennIndividual.h"

namespace Denn
{
    ////////////////////////////////////////////////////////////////////////
	//Population
	class Population
	{
	public:

		//pointer type
		using IndividualPtr = typename Individual::SPtr;

		//vector methods 		
		size_t size() const;
		void   resize(size_t i);
		void   push_back(const IndividualPtr& i);

		//vector operator
		IndividualPtr& operator[](size_t i);
		const IndividualPtr& operator[](size_t i) const;
		
		//iterator
		typename std::vector < IndividualPtr >::iterator       begin();
		typename std::vector < IndividualPtr >::const_iterator begin() const;
		typename std::vector < IndividualPtr >::iterator       end();
		typename std::vector < IndividualPtr >::const_iterator end() const;

		//costum
		void best(size_t& out_i, Scalar& out_eval) const;
		IndividualPtr best() const;
	
	protected:

		std::vector < IndividualPtr > m_individuals;

	};

    ////////////////////////////////////////////////////////////////////////
	enum class PopulationType : size_t
	{
		PT_PARENTS=0,
		PT_SONS,
		PT_SIZE
	};
	//Double Population buffer
	struct DoubleBufferPopulation
	{
        //Pointer
        using IndividualPtr  = typename Denn::Individual::SPtr;
        using RandomFunction = std::function<Scalar(Scalar)>;
		using CostFunction   = std::function<Scalar(const Matrix&, const Matrix&) >;
        //attributes
		Population m_pop_buffer[ size_t(PopulationType::PT_SIZE) ];
		//init population
		void init(
			size_t np
			, const IndividualPtr& i_default
			, const DataSet& dataset
			, const RandomFunction random_func
			, CostFunction target_function
		);
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
		IndividualPtr best() const;
		//swap
		void the_best_sons_become_parents();
		//restart
		void restart
		(
			  IndividualPtr best
			, const IndividualPtr& i_default
			, const DataSet& dataset
			, const RandomFunction random_func
			, CostFunction target_function
		);
	};
}