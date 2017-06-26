#include "DennIndividual.h"

namespace Denn
{
	//return ptr
	Individual::SPtr Individual::get_ptr() { return this->shared_from_this(); }
	//shared copy
	Individual::SPtr Individual::copy() const
	{
		return std::make_shared<Individual>(*this);
	}
	//init
	Individual::Individual() {}
	Individual::Individual(Scalar f, Scalar cr, const NeuralNetwork& network)
	{
		m_f = f;
		m_cr = cr;
		m_network = network;
	}
	//copy attributes from a other individual
	void Individual::copy_attributes(const Individual& i)
	{
		m_f = i.m_f;
		m_cr = i.m_cr;
		m_eval = i.m_eval;
	}
	//cast
	Individual::operator NeuralNetwork&()
	{
		return m_network;
	}

	Individual::operator const NeuralNetwork& () const
	{
		return m_network;
	}
	//like Network
	Layer& Individual::operator[](size_t i)
	{
		return m_network[i];
	}
	const Layer& Individual::operator[](size_t i) const
	{
		return m_network[i];
	}
	size_t Individual::size() const
	{
		return m_network.size();
	}
}