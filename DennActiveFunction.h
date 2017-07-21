#pragma once
#include "Config.h"

namespace Denn
{
	//type
	typedef Matrix& (*ActiveFunction)(Matrix& input);

	//class factory of ActiveFunction
	class ActiveFunctionFactory
	{

	public:

		//public
		static ActiveFunction get(const std::string& name);
		static void append(const std::string& name,const ActiveFunction& fun);

		//list of methods
		static std::vector< std::string > list_of_active_functions();
		static std::string names_of_active_functions(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	protected:

		//ActiveFunction map
		static std::unique_ptr< std::map< std::string, ActiveFunction > > m_cmap;

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
	
	#define __HELPER_BASE__REGISTERED_ACTIVE_FUNCTION(item_name_, function_ , name_)\
	namespace\
	{\
		static const ActiveFunctionItem& _Denn_ ## item_name_ ## _ActiveFunctionItem = ActiveFunctionItem(name_, function_);\
	}
	#define __HELPER_CALL__REGISTERED_ACTIVE_FUNCTION(item_name_, function_ , name_)\
		__HELPER_BASE__REGISTERED_ACTIVE_FUNCTION( item_name_, function_, name_)

	#define REGISTERED_ACTIVE_FUNCTION(function_ , name_)\
		__HELPER_CALL__REGISTERED_ACTIVE_FUNCTION( __COUNTER__, function_, name_)
}