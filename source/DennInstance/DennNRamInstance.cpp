#include "Config.h"
#include "DennInstance.h"
#include "DennEvaluation.h"
#include "DennRuntimeOutput.h"
#include "DennSerializeOutput.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include "DennNRamDatasetTask.h"
#include "DennInstanceUtils.h"
#include <fstream>

namespace Denn
{
namespace NRam
{
	//NRam
	class NRamEval : public Evaluation
	{
	public:
		//methods
		virtual bool minimize() const { return true; }
		virtual Scalar operator () (const Individual& individual, const DataSet& ds)
		{
			assert(m_context);
			//network
			auto& nn = individual.m_network;
			//Dataset
			auto& in_mem = ds.features();
			auto& out_mem = ds.labels();
			//regs
			auto regs = NRam::fuzzy_regs(m_context->m_batch_size, m_context->m_n_regs, m_context->m_max_int);
			//to list
			auto list_in_mem = NRam::fuzzy_encode(in_mem);
			//execute
			return NRam::train(*m_context, nn, regs, list_in_mem, out_mem);
		}
		//layout
		const NRam::NRamLayout* m_context { nullptr };
		//set context
		Evaluation::SPtr set_context(const NRamLayout& context)
		{
			m_context = &context;
			return this->get_ptr();
		}
	};
	REGISTERED_EVALUATION(NRamEval, "nram")

	class NRamInstance : public Instance
	{
	public:
		//local info
		std::ofstream  m_runtime_output_file_stream;  //n.b. before of context dec
		std::ofstream  m_serialize_output_file_stream;//n.b. before of context dec
		bool		   m_success_init{ false };
		//context
		const Denn::Parameters&	      m_parameters;    //< parameters
		mutable NRamLayout            m_nram;		   //< nram layout		
		mutable Task::SPtr			  m_task;	       //< nram task
		mutable DataSetTask           m_dataset;	   //< dataset
		mutable NRamEval::SPtr        m_eval;		   //< evaluator
		mutable NeuralNetwork         m_network;	   //< default network
		mutable Random				  m_random_engine; //< random engine
		mutable SerializeOutput::SPtr m_serialize;     //< serialize output
		mutable std::unique_ptr<ThreadPool> m_pool{ nullptr };     //< thread pool
		mutable std::ostream   m_runtime_output_stream{ nullptr }; //< stream
		//init
		NRamInstance(const Denn::Parameters& parameters) : m_parameters(parameters)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////
			//init
			m_random_engine.reinit(parameters.m_seed);
			////////////////////////////////////////////////////////////////////////////////////////////////
			//threads
			if (!build_thread_pool(m_pool, parameters)) return;
			//stream
			if (!build_outputstream(m_runtime_output_stream, m_runtime_output_file_stream, parameters)) return;
			//serialize
			if (!build_serialize(m_serialize, m_serialize_output_file_stream, parameters))  return;		
			////////////////////////////////////////////////////////////////////////////////////////////////
			//Create context
			#if 0
			m_nram.init(1000, 16, 2, 3,
			{
				GateFactory::create("read"),
				GateFactory::create("zero"),
				GateFactory::create("inc"),
				GateFactory::create("lt"),
				GateFactory::create("min"),
				GateFactory::create("write")
			});
			#else 
			m_nram.init
			(
			  1000 // Samples
			, 10   // Max int
			, 4    // Registers
			, 1    // Timesteps
			,{
				GateFactory::create("read"),
				GateFactory::create("inc"),
				GateFactory::create("add"),
				GateFactory::create("dec"),
				GateFactory::create("min"),
				GateFactory::create("write")
			 });
			#endif
			//get eval & set context
			m_eval = EvaluationFactory::get<NRamEval>("nram")->set_context(m_nram);
			//task
			m_task = TaskFactory::create("copy", m_nram.m_batch_size, m_nram.m_max_int, m_nram.m_n_regs, m_random_engine);
			//Dataset
			m_dataset = *m_task;
			//network
			m_network = build_mlp_network(m_nram.m_n_regs, m_nram.m_nn_output, parameters);
			////////////////////////////////////////////////////////////////////////////////////////////////
			m_success_init = true;
		}
		virtual ~NRamInstance()
		{
			//none	
		}

		Random&  random_engine()  const override
		{
			return m_random_engine;
		}

		const NeuralNetwork&  neural_network() const override
		{
			return m_network;
		}

		DataSetLoader& dataset_loader() const override
		{
			return m_dataset;
		}

		Evaluation::SPtr      loss_function() const override
		{
			return m_eval;
		}

		Evaluation::SPtr      validation_function() const override
		{
			return m_eval;
		}

		Evaluation::SPtr      test_function() const override
		{
			return m_eval;
		}

		std::ostream&         output_stream() const override
		{
			return m_runtime_output_stream;
		}

		SerializeOutput::SPtr serialize_output() const override
		{
			return m_serialize;
		}		
		
		ThreadPool*	thread_pool() const override
		{
			return m_pool.get();
		}

		bool execute() override
		{
			if(!m_success_init) return false;
			////////////////////////////////////////////////////////////////////////////////////////////////
			//DENN
			DennAlgorithm denn(*this, m_parameters);
			//execute
			double execute_time = Time::get_time();
			auto result = denn.execute();
			execute_time = Time::get_time() - execute_time;
			//output
			m_serialize->serialize_parameters(m_parameters);
			m_serialize->serialize_best
			(
				  execute_time
				, denn.execute_test(*result)
				, result->m_f
				, result->m_cr
				, result->m_network
			);
			//save best
			m_network = result->m_network;
			//success
			return true;
		}
	};
	REGISTERED_INSTANCE(NRamInstance, "nram")

}
}