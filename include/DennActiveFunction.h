#pragma once
#include "Config.h"

namespace Denn
{
	//type
	struct ActiveFunction 
	{
		typedef Matrix& (*Function)(Matrix& input);
		typedef Matrix& (*FunctionDerivate)(Matrix& input);
		//functions
		Function         m_function         { nullptr };
		FunctionDerivate m_function_derivate{ nullptr };
		//init
		ActiveFunction()
		{
		}
		ActiveFunction(Function function)
		: m_function(function)
		, m_function_derivate(nullptr)
		{
		}
		ActiveFunction(Function function, FunctionDerivate function_derivate)
		: m_function(function)
		, m_function_derivate(function_derivate)
		{
		}
		//actions
		const Matrix& operator() (Matrix& input) { return m_function(input); }
		const Matrix& apply(Matrix& input)  { return m_function(input); }
		const Matrix& derive(Matrix& input) { return m_function_derivate(input); }
		//operation
		bool operator == (const ActiveFunction& right) const
		{
			return m_function == right.m_function && m_function_derivate == right.m_function_derivate;
		}
		//helps
		operator bool() const                 { return exists_function(); }
		bool exists_function() const		  { return m_function != nullptr; }
		bool exists_function_derivate() const { return m_function_derivate != nullptr; }
	};


	//class factory of ActiveFunction
	class ActiveFunctionFactory
	{

	public:

		//public
		static ActiveFunction get(const std::string& name);
		static void append(const std::string& name,const ActiveFunction& fun);
		static std::string name_of(const ActiveFunction& fun);

		//list of methods
		static std::vector< std::string > list_of_active_functions();
		static std::string names_of_active_functions(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};

	//class used for static registration of a object class
	class ActiveFunctionItem
	{
	public:
		ActiveFunctionItem(const std::string& name,const ActiveFunction& active_function)
		{
			ActiveFunctionFactory::append(name, active_function);
		}
	};
	
	#define __HELPER_BASE__REGISTERED_ACTIVE_FUNCTION(item_name_, name_, ...)\
	namespace\
	{\
		static const ActiveFunctionItem& _Denn_ ## item_name_ ## _ActiveFunctionItem = ActiveFunctionItem(name_, ActiveFunction(__VA_ARGS__));\
	}
	#define __HELPER_CALL__REGISTERED_ACTIVE_FUNCTION(item_name_, name_, ... )\
		__HELPER_BASE__REGISTERED_ACTIVE_FUNCTION( item_name_, name_, __VA_ARGS__)

	#define REGISTERED_ACTIVE_FUNCTION(name_, ...)\
		__HELPER_CALL__REGISTERED_ACTIVE_FUNCTION( __COUNTER__, name_, __VA_ARGS__)
}