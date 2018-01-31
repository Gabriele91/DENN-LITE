#include "Config.h"
#include "DennInstance.h"
#include "DennEvaluation.h"
#include "DennRuntimeOutput.h"
#include "DennSerializeOutput.h"
#include "DennNRam.h"
#include "DennNRamTask.h"
#include "DennNRamDatasetTask.h"
#include "DennInstanceUtils.h"
#include "DennJson.h"
#include "DennDump.h"
#include <fstream>

namespace Denn
{
namespace NRam
{
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
			// Test
			if (!(*parameters.m_gates).size())
			{
				std::cerr << "nram required a list of gates" << std::endl;
				return;
			}
			if (!(*parameters.m_task).size())
			{
				std::cerr << "nram's learning process requires a task" << std::endl;
				return;
			}
			// gete list of gates
			std::vector< Gate::SPtr > gates;
			for (auto& gate_name : *(parameters.m_gates)) gates.push_back(GateFactory::create(gate_name));
			//init nram context
			m_nram.init
			(
				  *parameters.m_batch_size
				, *parameters.m_max_int
				, *parameters.m_n_registers
				, *parameters.m_time_steps
                , *parameters.m_registers_values_extraction_type != "defuzzyed" 
				  ? NRamLayout::P_ZERO 
				  : NRamLayout::P_DEFUZZYED
				, gates
			);
			//get eval & set context
			m_eval = EvaluationFactory::get<NRamEval>("nram")->set_context(m_nram);
			//task
			m_task = TaskFactory::create(*parameters.m_task, m_nram.m_batch_size, m_nram.m_max_int, m_nram.m_n_regs, 
				*parameters.m_time_steps, *parameters.m_min_difficulty, *parameters.m_max_difficulty, 
				*parameters.m_step_gen_change_difficulty, m_random_engine
			);
			//Dataset
			m_dataset = *m_task;
			//network
			m_network = build_mlp_network(m_nram.m_n_regs, m_nram.m_nn_output, parameters);
			//last layer must to be "linear"
			m_network[m_network.size()-1].set_activation_function(ActivationFunctionFactory::get("linear"));
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
