#include "DennMutation.h"
#include "DennParameters.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//Mutation
	Mutation::Mutation(const Parameters& parameters)
	: m_clamp
	([cmin = parameters.m_clamp_min
	 ,cmax = parameters.m_clamp_max]
	(Scalar value) -> Scalar
	{
		return Denn::clamp<Scalar>(value, cmin, cmax);
	})
	, m_parameters(parameters)
	{
	}

	//map
	std::unique_ptr< std::map< std::string, MutationFactory::CreateObject > > MutationFactory::m_cmap;
	//public
	Mutation::SPtr MutationFactory::create(const std::string& name, const Parameters& parameters)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(parameters);
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