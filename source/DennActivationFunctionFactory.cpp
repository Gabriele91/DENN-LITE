#include "Config.h"
#include "DennActivationFunction.h"
#include <iterator>

namespace Denn
{
    //map
	static std::map< std::string, ActivationFunction >& a_map()
	{
		static std::map< std::string, ActivationFunction > a_map;
		return a_map;
	}
	//public
	ActivationFunction ActivationFunctionFactory::get(const std::string& name)
	{
		//find
		auto it =  a_map().find(name);
		//return
		return it != a_map().end() ? it->second : nullptr;
	}
	void ActivationFunctionFactory::append(const std::string& name,const ActivationFunction& fun)
	{
		//find
		auto it = a_map().find(name);
		//add
		if (it == a_map().end()) a_map().insert({ name,fun });
		else					 a_map()[name] = fun;
	}
	std::string ActivationFunctionFactory::name_of(const ActivationFunction& fun)
	{
		//for all elements
		for (auto it : a_map()) if(it.second == fun) return it.first;
		//none
		return std::string();
	}
	//list of methods
	std::vector< std::string > ActivationFunctionFactory::list_of_activation_functions()
	{
		std::vector< std::string > list;
		for (const auto & pair : a_map()) list.push_back(pair.first);
		return list;
	}
	std::string ActivationFunctionFactory::names_of_activation_functions(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_activation_functions();
		std::copy(list.begin(), list.end()-1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	bool ActivationFunctionFactory::exists(const std::string& name)
	{
		//find
		auto it = a_map().find(name);
		//return 
		return it != a_map().end();
	}
}
