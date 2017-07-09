#include "DennMutation.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//Mutation
	Mutation::Mutation(const DennAlgorithm& algorithm)
	: m_algorithm(algorithm)
	{
	}

	//map
	std::unique_ptr< std::map< std::string, MutationFactory::CreateObject > > MutationFactory::m_cmap;
	//public
	Mutation::SPtr MutationFactory::create(const std::string& name, const DennAlgorithm& algorithm)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(algorithm);
	}
	void MutationFactory::append(const std::string& name, MutationFactory::CreateObject fun, size_t size)
	{
		//alloc
		if (!m_cmap) m_cmap = std::move(std::make_unique< std::map< std::string, MutationFactory::CreateObject > >());
		//add
		m_cmap->operator[](name) = fun;
	}
	//list of methods
	std::vector< std::string > MutationFactory::list_of_mutations()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string  MutationFactory::names_of_mutations(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_mutations();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	//info
	bool MutationFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}