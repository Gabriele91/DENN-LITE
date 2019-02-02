#include <iterator>
#include "DennLayer.h"

namespace Denn
{
	//shared this
	Layer::SPtr Layer::get_ptr() { return this->shared_from_this(); }
	///////////////////////////////////////////////////////////////////////////
	Layer::Iterator::Iterator(const Iterator& it)               :m_layer(it.m_layer), m_index(it.m_index) {}
	Layer::Iterator::Iterator(Layer& layer, size_t index)       :m_layer((Layer*)&layer), m_index(index)  {}
	Layer::Iterator::Iterator(const Layer& layer, size_t index) :m_layer((Layer*)&layer), m_index(index)  {}

	Layer::Iterator& Layer::Iterator::operator++()    { ++m_index; return *this; }
	Layer::Iterator  Layer::Iterator::operator++(int) { Iterator it(*this); operator++(); return it; }

	bool Layer::Iterator::operator==(const Layer::Iterator& rhs) const { return m_index == rhs.m_index; }
	bool Layer::Iterator::operator!=(const Layer::Iterator& rhs) const { return m_index != rhs.m_index; }

	Matrix& Layer::Iterator::operator*() { return (*m_layer)[m_index]; }
	const Matrix& Layer::Iterator::operator*() const { return (*((const Layer*)m_layer))[m_index]; }
	///////////////////////////////////////////////////////////////////////////
	Layer::Iterator Layer::begin()			   { return Iterator(*this, 0); }
	Layer::Iterator Layer::end()			   { return Iterator(*this, size()); }
	const Layer::Iterator Layer::begin() const { return Iterator(*this, 0); }
	const Layer::Iterator Layer::end()   const { return Iterator(*this, size()); }

	///////////////////////////////////////////////////////////////////////////
	// Factory
	struct LayerInfoLayer
	{
		LayerDescription m_description;
		LayerFactory::CreateObject m_create;
	}; 

	static std::map< std::string, LayerInfoLayer >& lr_map()
	{
		static std::map< std::string, LayerInfoLayer > lr_map;
		return lr_map;
	}

	static std::map< std::string, std::string >& lr_name_map()
	{
		static std::map< std::string, std::string > lr_name_map;
		return lr_name_map;
	}

	static void lr_intert(const std::vector<std::string>& names,const LayerInfoLayer& info)
	{
		//add names
		for(const std::string& name : names) 
			lr_name_map()[name] = names[0];
		//insert
		lr_map()[names[0]] = info;
	}

	static std::map< std::string, LayerInfoLayer >::iterator lr_find(const std::string& name)
	{
		//gate main name
		auto itname = lr_name_map().find(name);
		//test
		if (itname != lr_name_map().end())
			return lr_map().find(itname->second);
		//fail
		return lr_map().end();
	}
	
	//public
	Layer::SPtr LayerFactory::create
	(
		  const std::string& name
		, const Layer::Shape& shape
		, const Layer::Input& inputs
		, const Layer::VActivationFunction& active_functions
	)
	{
		//find
		auto it = lr_map().find(name);
		//return
		return it == lr_map().end() ? nullptr : it->second.m_create(shape, inputs, active_functions);
	}

	void LayerFactory::append(const std::string& name, CreateObject fun, const LayerDescription& ninput, size_t size)
	{
		//add
		lr_intert({ name }, LayerInfoLayer{ ninput, fun });
	}

	void LayerFactory::append(const std::vector<std::string>& names, CreateObject fun, const LayerDescription& ninput, size_t size)
	{
		//add
		lr_intert(names, LayerInfoLayer{ ninput, fun });
	}

	LayerDescription* LayerFactory::layer_description(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? nullptr : &it->second.m_description;

	}
	int LayerFactory::min_shape_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_shape.m_min;
	}
	int LayerFactory::max_shape_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_shape.m_max;
	}
	int LayerFactory::min_input_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_input.m_min;
	}
	int LayerFactory::max_input_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_input.m_max;
	}
	int LayerFactory::min_output_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_output.m_min;
	}
	int LayerFactory::max_output_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_output.m_max;
	}
	int LayerFactory::min_activation_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_function.m_min;
	}
	int LayerFactory::max_activation_size(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_function.m_max;
	}
	unsigned int LayerFactory::flags(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return
		return it == lr_map().end() ? 0 : it->second.m_description.m_flags;
	}
	//list of methods
	std::vector< std::string > LayerFactory::list_of_layers()
	{
		std::vector< std::string > list;
		for (const auto & pair : lr_map()) list.push_back(pair.first);
		return list;
	}
	std::string LayerFactory::names_of_layers(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_layers();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}

	//info
	bool LayerFactory::exists(const std::string& name)
	{
		//find
		auto it = lr_find(name);
		//return 
		return it != lr_map().end();
	}
}
