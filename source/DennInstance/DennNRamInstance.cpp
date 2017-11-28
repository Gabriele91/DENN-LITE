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
			auto& in_mem  = ds.features();
			auto& out_mem = ds.labels();
			//execute
			return NRam::train(*m_context, nn, in_mem, out_mem);
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
				, gates
			);
			//get eval & set context
			m_eval = EvaluationFactory::get<NRamEval>("nram")->set_context(m_nram);
			//task
			m_task = TaskFactory::create(*parameters.m_task, m_nram.m_batch_size, m_nram.m_max_int, m_nram.m_n_regs, m_random_engine);
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

	class NRamTestInstance : public Instance
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
		mutable DataSetTask           m_dataset;	   //< dataset //not used
		mutable NRamEval::SPtr        m_eval;		   //< evaluator
		mutable NeuralNetwork         m_network;	   //< default network
		mutable Random				  m_random_engine; //< random engine
		mutable std::ostream  		  m_runtime_output_stream{ nullptr }; //< stream
		mutable size_t				  m_tests{5};
		//init
		NRamTestInstance(const Denn::Parameters& parameters) : m_parameters(parameters)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////
			//init
			m_random_engine.reinit(parameters.m_seed);
			////////////////////////////////////////////////////////////////////////////////////////////////
			//stream
			if (!build_outputstream(m_runtime_output_stream, m_runtime_output_file_stream, parameters)) return;
			////////////////////////////////////////////////////////////////////////////////////////////////
			// test config file
			if (!(*parameters.m_dataset_filename).size()) return;
			//test file
			if (!Denn::Filesystem::exists((const std::string&)parameters.m_dataset_filename))
			{
				std::cerr << "input file: \"" << *parameters.m_dataset_filename << "\" not exists!" << std::endl;
				return; //exit
			}
			//json str
			std::string json_source = Filesystem::text_file_read_all(*parameters.m_dataset_filename);
			//load
			Json jdata(json_source);
			//errors?
			if(jdata.errors().size())
			{
				//error to parsing
				std::cerr << "json parse errors:" << std::endl;
				//print all errors
				std::cerr << jdata.errors() << std::endl;
				//end
				return; //exit
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			// gete list of gates
			std::vector< Gate::SPtr > gates;
			for (auto& gate_name : jdata["arguments"]["gates"].array())
			{
				gates.push_back(GateFactory::create(gate_name.string()));
			} 
			//init nram context
			m_nram.init
			(
				  1 //1 test
				, jdata["arguments"]["max_int"].number()
				, jdata["arguments"]["n_registers"].number()
				, jdata["arguments"]["time_steps"].number()
				, gates
			);
			//get eval & set context
			m_eval = EvaluationFactory::get<NRamEval>("nram")->set_context(m_nram);
			//task
			m_task = TaskFactory::create(jdata["arguments"]["task"].string(), 1, m_nram.m_max_int, m_nram.m_n_regs, m_random_engine);
			//network
			m_network = build_mlp_network(m_nram.m_n_regs, m_nram.m_nn_output, parameters);
			////////////////////////////////////////////////////////////////////////////////////////////////
			//test
		    auto& network = jdata["network"];
			for(size_t l = 0; l < m_network.size(); ++l)
			for(size_t m = 0; m < m_network[l].size(); ++m)
			for(size_t r = 0; r < m_network[l][m].rows(); ++r)
			for(size_t c = 0; c < m_network[l][m].cols(); ++c)
			{
				m_network[l][m](r,c) = network[l][m][r][c].number();
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			m_success_init = true;
		}
		virtual ~NRamTestInstance()
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
			return nullptr;
		}		
		
		ThreadPool*	thread_pool() const override
		{
			return nullptr;
		}

		bool execute() override
		{
			if(!m_success_init) return false;
			////////////////////////////////////////////////////////////////////////////////////////////////
			//for 5 test
			for(size_t t = 0; t!=m_tests; ++t)
			{
				//test
				auto test = (*m_task)();
				//Dataset
				auto& in_mem  = std::get<0>(test);
				auto& out_mem = std::get<1>(test);
				//print
				output_stream()  << "-----------------" << std::endl; 
				output_stream()  << "Test[" << t << "]" << std::endl; 
				output_stream() << "in: "  << Dump::json_matrix(in_mem) << std::endl;
				//execute
				NRam::train(m_nram, m_network, in_mem, out_mem);
				//output
				output_stream() << "out: " << Dump::json_matrix(out_mem) << std::endl;
				output_stream() << std::endl;;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			//success
			return true;
		}
	};
	REGISTERED_INSTANCE(NRamTestInstance, "nram_test")
}
}
