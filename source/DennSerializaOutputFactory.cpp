#include "DennSerializeOutput.h"
#include "DennParameters.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//map
	std::unique_ptr< std::map< std::string, SerializeOutputFactory::CreateObject > > SerializeOutputFactory::m_cmap;
	//public
	SerializeOutput::SPtr SerializeOutputFactory::create(const std::string& name, std::ostream& stream, const Parameters& params)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(stream, params);
	}
	void SerializeOutputFactory::append(const std::string& name, SerializeOutputFactory::CreateObject fun, size_t size)
	{
		//alloc
		if (!m_cmap) m_cmap = std::move(std::make_unique< std::map< std::string, SerializeOutputFactory::CreateObject > >());
		//add
		m_cmap->operator[](name) = fun;
	}
	//list of methods
	std::vector< std::string > SerializeOutputFactory::list_of_serialize_outputs()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string  SerializeOutputFactory::names_of_serialize_outputs(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_serialize_outputs();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	//info
	bool SerializeOutputFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}