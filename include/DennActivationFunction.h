#pragma once
#include "Config.h"

namespace Denn
{
	//type
	struct ActivationFunction 
	{
		using Activation = void (*)(const Matrix& Z, Matrix& A);
		using Jacobian = void (*)(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G);
		//functions
		Activation   m_activation{ nullptr };
		Jacobian	 m_jacobian{ nullptr };
		//init
		ActivationFunction()
		{
		}
		ActivationFunction(Activation activation, Jacobian jacobian)
		: m_activation(activation)
		, m_jacobian(jacobian)
		{
		}
		//actions
		void activate(const Matrix& Z, Matrix& A) const
		{ 
			m_activation(Z, A);
		}
		void jacobian(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G)  const 
		{ 
			m_jacobian(Z, A, F, G);
		}
		//operation
		bool operator == (const ActivationFunction& right) const
		{
			return m_activation == right.m_activation && m_jacobian == right.m_jacobian;
		}
	};


	//class factory of ActivationFunction
	class ActivationFunctionFactory
	{

	public:

		//public
		static ActivationFunction get(const std::string& name);
		static void append(const std::string& name,const ActivationFunction& fun);
		static std::string name_of(const ActivationFunction& fun);

		//list of methods
		static std::vector< std::string > list_of_activation_functions();
		static std::string names_of_activation_functions(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};

	//class used for static registration of a object class
	class ActivationFunctionItem
	{
	public:
		ActivationFunctionItem(const std::string& name,const ActivationFunction& active_function)
		{
			ActivationFunctionFactory::append(name, active_function);
		}
	};
	
	#define __HELPER_BASE_REGISTERED_ACTIVATION_FUNCTION(item_name_, name_, ...)\
	namespace\
	{\
		static const ActivationFunctionItem& _Denn_ ## item_name_ ## _ActivationFunctionItem = ActivationFunctionItem(name_, ActivationFunction(__VA_ARGS__));\
	}
	#define __HELPER_CALL_REGISTERED_ACTIVATION_FUNCTION(item_name_, name_, ... )\
		__HELPER_BASE_REGISTERED_ACTIVATION_FUNCTION( item_name_, name_, __VA_ARGS__)

	#define REGISTERED_ACTIVATION_FUNCTION(name_, ...)\
		__HELPER_CALL_REGISTERED_ACTIVATION_FUNCTION( __COUNTER__, name_, __VA_ARGS__)
}