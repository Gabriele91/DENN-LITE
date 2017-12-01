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
		mutable Evaluation::SPtr      m_eval;		   //< evaluator
		mutable NeuralNetwork         m_network;	   //< default network
		mutable Random				  m_random_engine; //< random engine
		mutable std::ostream  		  m_runtime_output_stream{ nullptr }; //< stream
		mutable size_t				  m_tests{1};
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
			// Number of input
			m_tests = parameters.m_n_test;
			////////////////////////////////////////////////////////////////////////////////////////////////
			// gete list of gates
			std::vector< Gate::SPtr > gates;
			for (auto& gate_name : jdata["arguments"]["gates"].array())
			{
				gates.push_back(GateFactory::create(gate_name.string()));
			} 			
			//test
			if (!gates.size())
			{
				//error to parsing
				std::cerr << "nram's gates are required" << std::endl;
				//end
				return; //exit
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
			//test
			if (!m_nram.m_max_int || !m_nram.m_n_regs || !m_nram.m_timesteps)
			{
				//error to parsing
				std::cerr << "nram's maxint, nram's registers and nram's timesteps must to be > 0" << std::endl;
				//end
				return; //exit
			}
			//get eval & set context
			m_eval = EvaluationFactory::get<NRamEval>("nram")->set_context(m_nram);
			//task
			m_task = TaskFactory::create(jdata["arguments"]["task"].string(), 1, m_nram.m_max_int, m_nram.m_n_regs, m_random_engine);
			//test
			if(!m_task)
			{
				//error to parsing
				std::cerr << "task: " << jdata["arguments"]["task"].string() << " not exists" << std::endl;
				//end
				return; //exit
			}
			//network
			m_network = build_mlp_network(m_nram.m_n_regs, m_nram.m_nn_output, parameters);
			m_network[m_network.size() - 1].set_active_function(ActiveFunctionFactory::get("linear"));
			////////////////////////////////////////////////////////////////////////////////////////////////
			//test
		    auto& network = jdata["network"];
			bool  parser_network_success = network.is_array();
			//parsing
			for (size_t l = 0; l < m_network.size() && parser_network_success; ++l)
			{
				//test
				parser_network_success = network[l].is_array();
				//for all layers
				for (size_t m = 0; m < m_network[l].size() && parser_network_success; ++m)
				{
					//test
					if (!(parser_network_success = network[l].is_array())) break;
					//get
					Matrix  matrix = matrix_from_json_array(network[l][m]);
					Matrix& nn_mat = m_network[l][m];
					//test
					if (nn_mat.rows() == matrix.rows() && nn_mat.cols() == matrix.cols())
					{
						//ok
						nn_mat = matrix;
					}
					else
					{
						//fail
						parser_network_success = false;
						break;
					}
				}
			}
			if (!parser_network_success)
			{
				//error to parsing
				std::cerr << "network configuration is wrong" << std::endl;
				//end
				return; //exit
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
			// Json output
			JsonArray jtests;
			// serialize?
			bool serialize = m_parameters.m_output_filename.get().size() > 0;
			////////////////////////////////////////////////////////////////////////////////////////////////
			//for each test
			for(size_t t = 0; t!=m_tests; ++t)
			{
				//test
				auto test = (*m_task)();
				//Dataset
				auto& in_mem  = std::get<0>(test);
				auto& out_mem = std::get<1>(test);
				//print
				output_stream()  << "Test[" << t << "]" << std::endl; 
				output_stream() << "in: "  << Dump::json_matrix(in_mem) << std::endl;
				//execute
				auto result = NRam::execute(m_nram, m_network, in_mem);
                auto output = std::get<0>(result);
                auto debug  = std::get<1>(result);
				//op shell
                output_stream() << debug[0].shell(); 
				//output
				output_stream() << "result: " << Dump::json_matrix(output) << std::endl;
				output_stream() << "expected out: " << Dump::json_matrix(out_mem) << std::endl;
				output_stream() << std::endl;
				//json
				if (serialize)
				{
					JsonObject jtest;
					jtest["memory"]    = json_array_from_matrix(in_mem);
					jtest["result"]    = json_array_from_matrix(output);
					jtest["expected"]  = json_array_from_matrix(out_mem);
					jtest["execution"] = debug[0].json().document();
					jtests.push_back(jtest);
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			// output
			if (serialize)
			{
				//open file
				std::ofstream jfile(*m_parameters.m_output_filename, std::ofstream::out);
				//write
				jfile << jtests;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			//success
			return true;
		}
	};
	REGISTERED_INSTANCE(NRamTestInstance, "nram_test")
}
}
