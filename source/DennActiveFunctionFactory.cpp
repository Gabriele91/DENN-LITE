#include "Config.h"
#include "DennActiveFunction.h"
#include <iterator>

namespace Denn
{
    //map
	std::unique_ptr< std::map< std::string, ActiveFunction > > ActiveFunctionFactory::m_cmap;
	//public
	ActiveFunction ActiveFunctionFactory::get(const std::string& name)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it != m_cmap->end() ? it->second : nullptr;
	}
	void ActiveFunctionFactory::append(const std::string& name,const ActiveFunction& fun)
	{
		//alloc
		if (!m_cmap) m_cmap = std::make_unique< std::map< std::string, ActiveFunction > >();
		//find
		auto it = m_cmap->find(name);
		//add
		if (it == m_cmap->end()) m_cmap->insert({ name,fun });
		else					 m_cmap->operator[](name) = fun;
	}
	std::string ActiveFunctionFactory::name_of(const ActiveFunction& fun)
	{
		//none
		if (!m_cmap) return std::string();
		//for all elements
		for (auto it : *m_cmap) if(it.second == fun) return it.first;
		//none
		return std::string();
	}
	//list of methods
	std::vector< std::string > ActiveFunctionFactory::list_of_active_functions()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string ActiveFunctionFactory::names_of_active_functions(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_active_functions();
		std::copy(list.begin(), list.end()-1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	bool ActiveFunctionFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}
