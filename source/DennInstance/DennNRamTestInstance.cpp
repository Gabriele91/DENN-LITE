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
		Json 		   m_jdata;
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
			// Number of input
			m_tests = parameters.m_n_test;
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
			m_jdata = Json(json_source);
			//errors?
			if(m_jdata.errors().size())
			{
				//error to parsing
				std::cerr << "json parse errors:" << std::endl;
				//print all errors
				std::cerr << m_jdata.errors() << std::endl;
				//end
				return; //exit
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			// arguments
			if(!m_jdata.document().is_object())
			{
				//error to parsing
				std::cerr << "json not valid" << std::endl;
				//end
				return; //exit
			}			
			// is a object json
			if(m_jdata.document().object().find("arguments") == m_jdata.document().object().end())
			{
				//error to parsing
				std::cerr << "not find \'arguments\' in json" << std::endl;
				//end
				return; //exit
			}
			//ref to args
			auto& jarguments = m_jdata["arguments"].object();
			////////////////////////////////////////////////////////////////////////////////////////////////
			//test arguments
			std::vector< std::string > name_of_args
			{
				"gates",
				"max_int",
				"n_registers",
				"task",
				"hidden_layers",
				"activation_functions",
			};
			for(const std::string& arg_name : name_of_args)
			{
				if(jarguments.find(arg_name) == jarguments.end())
				{
					//error to parsing
					std::cerr << "not find \'arguments." << arg_name << "\' in json" << std::endl;
					//end
					return; //exit
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			// gete list of gates
			std::vector< Gate::SPtr > gates;
			for (auto& gate_name : jarguments["gates"].array())
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
			//nram context info
			size_t nram_max_int = jarguments["max_int"].number();
			size_t nram_n_registers = jarguments["n_registers"].number();
			size_t nram_time_steps = jarguments["time_steps"].number();
			size_t nram_registers_values_extract = jarguments["registers_values_extract"].number();
			Scalar nram_entropy_term = jarguments["entropy_term"].number();
			Scalar nram_entropy_decay = jarguments["entropy_decay"].number();
			Scalar nram_cost_regularization_term = jarguments["cost_regularization_term"].number();
			//if nram_time_steps == 0 || nram_max_int==0 try to find value in metadata
			if(!nram_time_steps || !nram_max_int)
			{
				//document
				auto& document = m_jdata.document().object();
				//find metadata
				auto it_metadata = document.find("metadata");
				//test
				if(it_metadata != document.end() && it_metadata->second.is_object())
				{
					//get metadata
					auto& metadata = it_metadata->second.object();
					//time_steps
					if (!nram_time_steps)
					{
						//get time_steps
						auto it_time_steps = metadata.find("time_steps");
						//test
						if (it_time_steps != metadata.end() && it_time_steps->second.is_number())
						{
							nram_time_steps = it_time_steps->second.number();
						}
					}
					//max int
					if (!nram_max_int)
					{
						//get max_int
						auto it_max_int = metadata.find("max_int");
						//test
						if (it_max_int != metadata.end() && it_max_int->second.is_number())
						{
							nram_max_int = it_max_int->second.number();
						}
					}
				}
			}
			//init nram context
			m_nram.init
			(
				  1 //1 test
				, nram_max_int
				, nram_n_registers
				, nram_time_steps
				, nram_registers_values_extract
				, nram_entropy_term
				, nram_entropy_decay
				, nram_cost_regularization_term
				, gates
				, false
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
			m_task = TaskFactory::create(jarguments["task"].string(), 1, m_nram.m_max_int, m_nram.m_n_regs, m_nram.m_timesteps, 1, 5, 100, 0, m_random_engine);
			//test
			if(!m_task)
			{
				//error to parsing
				std::cerr << "task: " << jarguments["task"].string() << " not exists" << std::endl;
				//end
				return; //exit
			}
			//network
			std::vector<unsigned int> hl_size;
			std::vector<std::string> hl_afun;
			auto& jhl_size = jarguments["hidden_layers"];
			auto& jhl_afun = jarguments["activation_functions"];
			bool  parser_network_success =  jhl_size.is_array() 
			                             && jhl_afun.is_array() 
									     && jhl_size.array().size() == jhl_afun.array().size();
			for (size_t l = 0; l != jhl_size.array().size() && parser_network_success; ++l)
			{
				hl_size.push_back(jhl_size.array()[l].number());
				hl_afun.push_back(jhl_afun.array()[l].string());
			}
			//
			if(!parser_network_success)
			{
				//error to parsing
				std::cerr << "network configuration is wrong" << std::endl;
				//end
				return; //exit
			}
			//build network
			m_network = build_mlp_network(m_nram.m_n_regs, m_nram.m_nn_output, hl_size, hl_afun, "linear");
			////////////////////////////////////////////////////////////////////////////////////////////////
			// Network
			if (m_jdata.document().object().find("network") == m_jdata.document().object().end())
			{
				//error to parsing
				std::cerr << "not find \'network\' in json" << std::endl;
				//end
				return; //exit
			}
		    auto& network = m_jdata["network"];
			bool  parser_matrix_network_success = network.is_array();
			//parsing
			for (size_t l = 0; l < m_network.size() && parser_matrix_network_success; ++l)
			{
				//test
				parser_matrix_network_success = network[l].is_array();
				//for all layers
				for (size_t m = 0; m < m_network[l].size() && parser_matrix_network_success; ++m)
				{
					//test
					if (!(parser_matrix_network_success = network[l].is_array())) break;
					//get
					Matrix matrix = matrix_from_json_array(network[l][m]);
					//test
					if (m_network[l][m].rows() == matrix.rows() && m_network[l][m].cols() == matrix.cols())
					{
						//ok
						m_network[l][m].array() = matrix.array();
					}
					else
					{
						//fail
						parser_matrix_network_success = false;
						break;
					}
				}
			}
			if (!parser_matrix_network_success)
			{
				//error to parsing
				std::cerr << "network's matrix configuration is wrong" << std::endl;
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
				const auto& test = (*m_task).create_batch(0, 0);
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
				//json to serialize
				JsonObject jexecution;
				//save tests
				jexecution["tests"] = jtests;
				//saveadd layout
				jexecution["layout"] = JsonObject();
				jexecution["layout"]["gates"] = m_jdata["arguments"]["gates"];
				jexecution["layout"]["max_int"] = m_jdata["arguments"]["max_int"];
				jexecution["layout"]["n_registers"] = m_jdata["arguments"]["n_registers"];
				jexecution["layout"]["time_steps"] = m_jdata["arguments"]["time_steps"];
				//write
				jfile << jexecution;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////
			//success
			return true;
		}
	};
	REGISTERED_INSTANCE(NRamTestInstance, "nram_test")
}
}
