#include "DennRuntimeOutput.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn 
{
    //map
	std::unique_ptr< std::map< std::string, RuntimeOutputFactory::CreateObject > > RuntimeOutputFactory::m_cmap;
	//easy access		
	const EvolutionMethod&        RuntimeOutput::evolution_method() const{ return m_algorithm.evolution_method();  }
	const Parameters&             RuntimeOutput::parameters()       const{ return m_algorithm.parameters();        }
	const DoubleBufferPopulation& RuntimeOutput::population()       const{ return m_algorithm.population(); }
	const size_t                  RuntimeOutput::current_np()       const{ return m_algorithm.current_np(); }
	//public
	RuntimeOutput::SPtr RuntimeOutputFactory::create(const std::string& name, std::ostream& stream, const DennAlgorithm& algorithm)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(stream,algorithm);
	}
	void RuntimeOutputFactory::append(const std::string& name, RuntimeOutputFactory::CreateObject fun, size_t size)
	{
		//alloc
		if (!m_cmap) m_cmap = std::move(std::make_unique< std::map< std::string, RuntimeOutputFactory::CreateObject > >());
		//add
		m_cmap->operator[](name) = fun;
	}
	//list of methods
	std::vector< std::string > RuntimeOutputFactory::list_of_runtime_outputs()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string  RuntimeOutputFactory::names_of_runtime_outputs(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_runtime_outputs();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	//info
	bool RuntimeOutputFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}