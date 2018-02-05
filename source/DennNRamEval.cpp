#include "DennNRam.h"

namespace Denn
{
namespace NRam 
{
    ////////////////////////////////////////////////////////////////////////////////////////
	//Evaluetor of nram
	REGISTERED_EVALUATION(NRamEval, "nram")
	//methods
	bool NRamEval::minimize() const { return true; }
	Scalar NRamEval::operator () (const Individual& individual, const DataSet& dataset)
	{
		denn_assert(m_context);
		//network
		auto& nn = individual.m_network;
		//Dataset
		auto& in_mem = dataset.features();
		auto& out_mem = dataset.labels();
		auto& cost_mask = dataset.mask();
        auto& max_int = dataset.get_metadata("max_int").get<int>();
        auto& timesteps = dataset.get_metadata("time_steps").get<int>();
		//execute
        return std::abs(NRam::train(*m_context, nn, in_mem, out_mem, cost_mask, max_int, timesteps));
	}
	//set context
	Evaluation::SPtr NRamEval::set_context(const NRamLayout& context)
	{
		m_context = &context;
		return this->get_ptr();
	}
}
}