#include "DennNRamEnhanced.h"

namespace Denn
{
namespace NRamEnhanced
{
    ////////////////////////////////////////////////////////////////////////////////////////
	//Evaluetor of nram
	REGISTERED_EVALUATION(NRamEval, "nram_enhanced")
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

		int& max_int = dataset.get_metadata("max_int").get<int>();
		int& timesteps = dataset.get_metadata("time_steps").get<int>();
		Matrix train_in_mem;
		Matrix test_in_mem;
		Matrix train_out_mem;
		Matrix test_out_mem;

		if (m_context->m_activate_curriculum_learning)
		{
			train_in_mem = in_mem.block(0, 0, batch_size - (batch_size / 10), max_int);
			test_in_mem = in_mem.block(batch_size - (batch_size / 10), 0, (batch_size / 10), max_int);
			train_out_mem = out_mem.block(0, 0, batch_size - (batch_size / 10), max_int);
			test_out_mem = out_mem.block(batch_size - (batch_size / 10), 0, (batch_size / 10), max_int);
		}
		else
		{
			train_in_mem = in_mem;
			train_out_mem = out_mem;
		}
		auto& cost_mask = dataset.mask();
		auto& error_m = dataset.mask_error();
		Matrix error_mask = error_m.block(batch_size - (batch_size / 10), 0, (batch_size / 10), max_int); // TODO

		// Execute
		Scalar train_result = NRamEnhanced::train(*m_context, nn, train_in_mem, train_out_mem, cost_mask, max_int, timesteps);
		
		// Set error rate
		if (m_context->m_activate_curriculum_learning)
			dataset.m_metadata["error_rate"] = NRamEnhanced::calculate_error_rate(*m_context, nn, test_in_mem, test_out_mem, cost_mask, error_mask, max_int, timesteps);
		else
			dataset.m_metadata["error_rate"] = Scalar(0.0);

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