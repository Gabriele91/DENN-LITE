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
		const auto& in_mem = dataset.features();
		const auto& out_mem = dataset.labels();

		const size_t batch_size = m_context->m_batch_size;

        auto& max_int = dataset.get_metadata("max_int").get<int>();
        auto& timesteps = dataset.get_metadata("time_steps").get<int>();
		auto& train_in_mem = in_mem.block(0, 0, batch_size - (batch_size / 10), max_int);
		auto& test_in_mem = in_mem.block(batch_size - (batch_size / 10), 0, (batch_size / 10), max_int);
		auto& train_out_mem = out_mem.block(0, 0, batch_size - (batch_size / 10), max_int);
		auto& test_out_mem = out_mem.block(batch_size - (batch_size / 10), 0, (batch_size / 10), max_int);
		auto& cost_mask = dataset.mask();

		// Execute
        Scalar train_result = 
			NRam::train(*m_context, nn, train_in_mem, train_out_mem, cost_mask, max_int, timesteps);
		
		// Set error rate
		dataset.m_metadata["error_rate"] = 
			NRam::calculate_error_rate(*m_context, nn, test_in_mem, test_out_mem, cost_mask, max_int, timesteps);

		return train_result;
	}
	//set context
	Evaluation::SPtr NRamEval::set_context(const NRamLayout& context)
	{
		m_context = &context;
		return this->get_ptr();
	}
}
}