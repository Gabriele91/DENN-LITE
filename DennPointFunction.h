#pragma once 
#include "Config.h"
#include "DennConstants.h"

namespace Denn
{
	namespace PointFunction
	{
		template < typename ScalarType >
		using Ptr = std::function< ScalarType(const ScalarType& input) >;

		template < typename ScalarType = double >
		inline ScalarType identity(const ScalarType& a)
		{
			return a;
		}

		template < typename ScalarType = double >
		inline ScalarType linear(const ScalarType& a)
		{
			return a;
		}

		template < typename ScalarType = double >
		inline ScalarType sigmoid(const ScalarType& a)
		{
			return ScalarType(1.0) / (ScalarType(1.0) + std::exp(-a));
		}

		template < typename ScalarType = double >
		inline ScalarType logistic(const ScalarType& a)
		{
			return ScalarType(1.0) / (ScalarType(1.0) + std::exp(-a));
		}	
		
		template < typename ScalarType = double >
		inline ScalarType log(const ScalarType& a)
		{
			return std::log(a);
		}

		template < typename ScalarType = double >
		inline ScalarType logit(const ScalarType& a)
		{
			return log<ScalarType>(a / (ScalarType(1.0) + a));
		}

		template < typename ScalarType = double >
		inline ScalarType tanh(const ScalarType& a)
		{
			return std::tanh(a);
		}

		template < typename ScalarType = double >
		inline ScalarType relu(const ScalarType& a)
		{
			return std::max(a, ScalarType(0));
		}

		template < typename ScalarType = double >
		inline ScalarType binary(const ScalarType& a)
		{
			return (a < ScalarType(0) ?  ScalarType(0) :  ScalarType(1));
		}
	}
}