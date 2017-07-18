#include "Layer.h"

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
}