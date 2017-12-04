#pragma once
#include "Config.h"

namespace Denn
{
	//type
	struct ActivationFunction 
	{
		typedef Matrix& (*Function)(Matrix& input);
		typedef Matrix& (*FunctionDerivate)(Matrix& input);
		//functions
		Function         m_function         { nullptr };
		FunctionDerivate m_function_derivate{ nullptr };
		//init
		ActivationFunction()
		{
		}
		ActivationFunction(Function function)
		: m_function(function)
		, m_function_derivate(nullptr)
		{
		}
		ActivationFunction(Function function, FunctionDerivate function_derivate)
		: m_function(function)
		, m_function_derivate(function_derivate)
		{
		}
		//actions
		const Matrix& operator() (Matrix& input) const { return m_function(input); }
		const Matrix& apply(Matrix& input)       const { return m_function(input); }
		const Matrix& derive(Matrix& input)      const { return m_function_derivate(input); }
		//operation
		bool operator == (const ActivationFunction& right) const
		{
			return m_function == right.m_function && m_function_derivate == right.m_function_derivate;
		}
		//helps
		operator bool() const                 { return exists_function(); }
		bool exists_function() const		  { return m_function != nullptr; }
		bool exists_function_derivate() const { return m_function_derivate != nullptr; }
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