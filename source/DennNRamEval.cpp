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
	Scalar NRamEval::operator () (const Individual& individual, const DataSet& d)
	{
		denn_assert(m_context);

		auto& dataset = *((DataSetScalar*)&d);

		// Network
		auto& nn = individual.m_network;
		
		// Dataset
		auto& in_mem = dataset.features();
		auto& out_mem = dataset.labels();
		auto& cost_mask = dataset.mask();
        auto& max_int = dataset.get_metadata("max_int").get<int>();
        auto& timesteps = dataset.get_metadata("time_steps").get<int>();

		// Execute
        auto train_result = NRam::train(*m_context, nn, in_mem, out_mem, cost_mask, max_int, timesteps);
		
		// Set error rate
		dataset.m_metadata["error_rate"] = std::get<1>(train_result);

		return std::get<0>(train_result);
	}
	//set context
	Evaluation::SPtr NRamEval::set_context(const NRamLayout& context)
	{
		m_context = &context;
		return this->get_ptr();
	}
}
}