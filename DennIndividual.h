#pragma once
#include "Config.h"
#include "CostFunction.h"
#include "NeuralNetwork.h"

namespace Denn
{
	////////////////////////////////////////////////////////////////////////
	class Individual : public std::enable_shared_from_this< Individual >
	{
	public:
		//ref to individual
		using SPtr = std::shared_ptr<Individual>;
		//return ptr
		SPtr get_ptr();
		//shared copy
		SPtr copy() const;
		//attributes
		Scalar m_eval{ std::numeric_limits<Scalar>::max() };
		Scalar m_f{ 1.0 };
		Scalar m_cr{ 1.0 };
		NeuralNetwork m_network;
		//init
		Individual();
		Individual(Scalar f, Scalar cr, const NeuralNetwork& network);
		//copy attributes from a other individual
		void copy_from(const Individual& individual);
		void copy_attributes(const Individual& individual);
		//cast
		explicit operator NeuralNetwork&();
		explicit operator const NeuralNetwork& () const;
		//like Network
		Layer& operator[](size_t i);
		const Layer& operator[](size_t i) const;
		size_t size() const;
	};
}