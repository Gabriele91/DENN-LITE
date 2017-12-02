#include "DennParameters.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennActiveFunction.h"
#include "DennEvolutionMethod.h"
#include "DennRuntimeOutput.h"
#include "DennSerializeOutput.h"
#include "DennInstance.h"
#include "DennDump.h"
#include "DennNRamGate.h"
#include "DennNRamTask.h"
#include "DennVersion.h"
#include <sstream>
#include <iostream>

namespace Denn
{     
    Parameters::Parameters() 
    :m_params_info
    ({
        ParameterInfo{
            m_instance, "Type of instance", { "--test_type", "-ins", "-tt" },
            [this](Arguments& args) -> bool  
            { 
                std::string str_m_type = args.get_string() ;
                //all lower case
                std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::tolower);
                //save
				m_instance = str_m_type;
                //ok 
                return InstanceFactory::exists(*m_instance);
            }
            , { "string", InstanceFactory::list_of_instances() }
        },
        ParameterInfo{ 
            m_generations, "Global number of generation [or backpropagation pass]", { "-t", "-g"  }
        },
        ParameterInfo{ 
            m_sub_gens, "Number of generation [or backpropagation pass] per batch", { "-s" , "-sg" }
        },
        ParameterInfo{
            m_np, "Number of parents", { "-np"  }
        },
        ParameterInfo{
            m_seed, "Random generator seed", { "-sd"  }
        },
        ParameterInfo{
            m_batch_size, "Batch size", { "--" + m_batch_size.name(), "-b" }
        },
        ParameterInfo{
            m_batch_offset, "Batch offset, how many records will be replaced in the next batch [<= batch size]", { "-bo" }
        },
        ParameterInfo{
            m_use_validation, "Use the validation test", { "-uv" }
        },
        ParameterInfo{
            m_evolution_type, "Type of evolution method", { "--evolution",    "-em"  },
            [this](Arguments& args) -> bool  
            { 
                std::string str_m_type = args.get_string() ;
                //all lower case
                std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::toupper);
                //save
				m_evolution_type = str_m_type;
                //ok 
                return EvolutionMethodFactory::exists(*m_evolution_type);
            }
            , { "string", EvolutionMethodFactory::list_of_evolution_methods() }
        },
        ParameterInfo{
              m_sub_evolution_type
            , "Type of sub evolution method (PHISTORY)"
            , { "--sub-evolution",    "-sem"  }
            , [this](Arguments& args) -> bool
              {
                  std::string str_m_type = args.get_string() ;
                  //all lower case
                  std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::toupper);
                  //save
				  m_sub_evolution_type = str_m_type;
                  //ok
                  return EvolutionMethodFactory::exists(*m_sub_evolution_type);
              }
            , { "string", EvolutionMethodFactory::list_of_evolution_methods() }
        },
        ParameterInfo{
              m_mutation_type
            , "Type of DE mutation"
            , { "-m"  }
            , [this](Arguments& args) -> bool
              {
                  std::string str_m_type = args.get_string() ;
                  //all lower case
                  std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::tolower);
                  //save
                  m_mutation_type = str_m_type;
                  //ok
                  return MutationFactory::exists(*m_mutation_type);
              }
            , { "string", MutationFactory::list_of_mutations() }
        },
        ParameterInfo{
              m_crossover_type
            , "Type of DE crossover"
            , { "-co"  }
            , [this](Arguments& args) -> bool
              {
                  std::string str_c_type = args.get_string();
                  //all lower case
                  std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                  //save
                  m_crossover_type = str_c_type;
                  //ok
                  return CrossoverFactory::exists(*m_crossover_type);
              }
            , { "string", CrossoverFactory::list_of_crossovers() }
        },

        ParameterInfo{
            m_history_size, "Size of population history (PHISTORY)", { "-hs"  }
        },

        ParameterInfo{ 
            m_default_f, "Default F factor for DE", { "-f"  }
        },
        ParameterInfo{ 
            m_default_cr, "Default CR factor for DE", { "-cr" }
        },
                            
        ParameterInfo{ 
            m_jde_f, "Probability of change of F (JDE)", { "-jf"  }
        },
        ParameterInfo{ 
            m_jde_cr, "Probability of change of CR (JDE)", { "-jcr"  }
        },
				
		ParameterInfo{
			m_archive_size, "Archive size (JADE/SHADE)",{ "-as" }
		},				
		ParameterInfo{
			m_f_cr_adapt, "Auto adaptation factor of f and c parameters (JADE)",{ "-afcr" }
		},

		ParameterInfo{
			m_shade_h, "Size of archive of mu_f and mu_cr in SHADE",{ "-shah" }
		}, 

		ParameterInfo{
			m_min_np, "Minimum number of parents (L-SHADE)",{ "-mnp" }
		},
		ParameterInfo{
				m_max_nfe, "Maximum number of fitness evaluation (L-SHADE)",{ "-mnfe" }
		},
		ParameterInfo{
			m_mu_cr_terminal_value, "Terminal value of MU Cr (L-SHADE)",{ "-mcrtv" }
		},

		ParameterInfo{
			m_perc_of_best, "Percentage of best (Current P Best/NO [L-]SHADE)",{ "-pob", "-p" }
		},	

		ParameterInfo{
			m_degl_scalar_weight, "Scalar weight used to combinate local mutant with global mutant (DEGL)",{ "-glw" }
		},				
		ParameterInfo{
			m_degl_neighborhood, "Neighborhood of target where is searched the best (DEGL)",{ "-glnn" }
		},	
		ParameterInfo{
			m_trig_m, "Probability of the mutation to be trigonometric (TDE)",{ "-tm" }
        },
        
        ParameterInfo{ 
            m_clamp_min, "Minimum size of weight", { "-cmin"  }
        },
        ParameterInfo{ 
            m_clamp_max, "Maximum size of weight", { "-cmax"  }
        },
        ParameterInfo{ 
            m_range_min, "Minimum size of weight in random initialization", { "-rmin"  }
        },
        ParameterInfo{ 
            m_range_max, "Maximum size of weight in random initialization", { "-rmax"  }
        },
        ParameterInfo{ 
            m_restart_count, "Number of change of batches before restart (if accuracy not increase)", { "-rc"  }, 
            [this](Arguments& args) -> bool  
            { 
                m_restart_count = args.get_int() ; 
                if(*m_restart_count < 0) m_restart_enable = false; 
                return true; 
            } 
        },
        ParameterInfo{ 
            m_restart_delta, "Delta factor to determine if accuracy is increased", { "-rd"  }, 
            [this](Arguments& args) -> bool 
            {
                m_restart_delta = args.get_double() ; 
                if(*m_restart_delta<0) m_restart_enable = false;
                 return true; 
            } 
        },
        ParameterInfo{ 
			m_learning_rate, "Learning rate of backpropagation for each pass", { "-lrbp"  },
            [this](Arguments& args) -> bool 
            {
				m_learning_rate = args.get_double() ;
                return Scalar(0.0) <= *m_learning_rate;
            } 
        },
        ParameterInfo{ 
			m_regularize, "Regularize of backpropagation for each pass", { "-rbp"  },
            [this](Arguments& args) -> bool 
            {
				m_regularize = args.get_double() ;
                return Scalar(0.0) <= *m_regularize;
            } 
        },
        ParameterInfo{
            m_hidden_layers, "Size of hidden layers", { "-hl"  },
            [this](Arguments& args) -> bool 
            { 
                while(args.remaining() && !args.start_with_minus())
                {
                    m_hidden_layers.get().push_back(args.get_int());
                }
                //ok
                return m_hidden_layers.get().size() != 0; 
            }
        },
        ParameterInfo{
            m_active_functions, "Activation functions of hidden layers", { "--activation_functions" ,  "-hlaf"  },
            [this](Arguments& args) -> bool 
            { 
                while(args.remaining() && !args.start_with_minus())
                {
                    std::string str_c_type = args.get_string();
                    //all lower case
                    std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                    //test
                    if(!ActiveFunctionFactory::exists(str_c_type)) return false;
                    //push
                    m_active_functions.get().push_back(str_c_type);
                }
                //ok
                return m_active_functions.get().size() != 0; 
            }
			,{ "list(string)",  ActiveFunctionFactory::list_of_active_functions() }
        },
        ParameterInfo{
			m_output_active_function, "Activation function of output layer", { "--output_activation_functions" ,  "-oaf"  },
            [this](Arguments& args) -> bool 
            { 
                std::string str_c_type = args.get_string();
                //all lower case
                std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                //test
                if(!ActiveFunctionFactory::exists(str_c_type)) return false;
                //save
				m_output_active_function = str_c_type;
                //ok
                return true;
            }
			,{ "string",  ActiveFunctionFactory::list_of_active_functions() }
        },
		ParameterInfo{
			m_max_int, "Max int of nram's registers",{ "-nrmi" }
		},
		ParameterInfo{
			m_n_registers, "Number of nram's registers",{ "-nrr" }
		},
		ParameterInfo{
			m_time_steps, "Time steps of nram machine",{ "-nrts" }
		},
        ParameterInfo{
			m_task, "Task of nram machine", { "-nrtk"  },
            [this](Arguments& args) -> bool 
            { 
                std::string str_c_type = args.get_string();
                //all lower case
                std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                //test
                if(!NRam::TaskFactory::exists(str_c_type)) return false;
                //save
				m_task = str_c_type;
                //ok
                return true;
            }
			,{ "string",  NRam::TaskFactory::list_of_tasks() }
        }, 
        ParameterInfo{
			m_gates, "List of gates of nram machine", { "-nrg"  },
            [this](Arguments& args) -> bool 
            { 
                while(args.remaining() && !args.start_with_minus())
                {
                    std::string str_c_type = args.get_string();
                    //all lower case
                    std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                    //test
                    if(!NRam::GateFactory::exists(str_c_type)) return false;
                    //push
					m_gates.get().push_back(str_c_type);
                }
                //ok
                return m_gates.get().size() != 0;
            }
			,{ "list(string)",  NRam::GateFactory::list_of_gates() }
        },
		ParameterInfo{
			m_n_test, "Number of test of nram",{ "-nrnt" }
		},				
        ParameterInfo{
            m_dataset_filename, "Path of dataset file (gz) or network file input (json)", { "-d", "-i" }
        },
        ParameterInfo{ 
            m_output_filename, "Path of output file (json)", { "-o" } 
        },
        ParameterInfo{
            m_runtime_output_type, "Select type of runtime output", { "-ro"  }, 
            [this](Arguments& args) -> bool 
			{ 
				m_runtime_output_type = args.get_string() ; 
				return true;
			}
			,{ "string",  NRam::TaskFactory::list_of_tasks() }
        },
        ParameterInfo{
            m_runtime_output_file, "Write the runtime output to stream [::cout, ::cerr, <file>]", { "-rof"  }
        },
        ParameterInfo{
            m_compute_test_per_pass, "Compute the test accuracy for each pass", { "-ctps"  }
        },
        ParameterInfo{
            m_serialize_neural_network, "Serialize the neural network of best individual", { "-snn"  }
        },

        ParameterInfo{ 
            m_threads_omp, "Number of threads using by OpenMP", { "-omp"  }
        },
        ParameterInfo{ 
            m_threads_pop, "Number of threads using for  generate a new population", { "-tp"  }
        },
        ParameterInfo{
            "Print list of instances", { "--instances-list", "-ilist"  }, 
            [this](Arguments& args) -> bool { std::cout << InstanceFactory::names_of_instances() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of evolution methods", { "--evolution_method-list", "--evolution-list",    "-elist"  }, 
            [this](Arguments& args) -> bool { std::cout << EvolutionMethodFactory::names_of_evolution_methods() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of muations", { "--mutation-list",    "-mlist"  }, 
            [this](Arguments& args) -> bool { std::cout << MutationFactory::names_of_mutations() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of crossovers", { "--crossover-list",    "-colist"  }, 
            [this](Arguments& args) -> bool { std::cout << CrossoverFactory::names_of_crossovers() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of activation functions", { "--active_functions-list", "--activation_functions-list",    "-aflist"  }, 
            [this](Arguments& args) -> bool { std::cout << ActiveFunctionFactory::names_of_active_functions() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of nram's taks", { "--nram-task-list", "--task-list", "-nrtlist"  },
            [this](Arguments& args) -> bool { std::cout << NRam::TaskFactory::names_of_tasks() << std::endl; return true; }
        },
        ParameterInfo{
            "Print list of nram's gates", { "--nram-gates-list", "--gates-list", "-nrglist"  },
            [this](Arguments& args) -> bool { std::cout << NRam::GateFactory::names_of_gates() << std::endl; return true; }
        },
        ParameterInfo{
            "Print list of runtime output", { "--runtime_output-list",    "-rolist"  }, 
            [this](Arguments& args) -> bool { std::cout << RuntimeOutputFactory::names_of_runtime_outputs() << std::endl; return true; } 
        },
		ParameterInfo{
			"Print list of serializer",{ "--serializer-list",    "-slist" },
			[this](Arguments& args) -> bool { std::cout << SerializeOutputFactory::names_of_serialize_outputs() << std::endl; return true; }
		},
        ParameterInfo{
            "Print the help", { "--help",    "-h"  },
            [this](Arguments& args) -> bool { std::cout << make_help();  return true; }
        },
        ParameterInfo{
            "Print the json help", { "--jhelp",    "-jh"  },
            [this](Arguments& args) -> bool { std::cout << make_help_json();  return true; }
        },
        ParameterInfo{
            "Displays the program's version", { "--version",    "-v"  },
            [this](Arguments& args) -> bool { std::cout << "version: " << DENN_VER_STR << std::endl;  return true; }
        }
    })
    {
    }

    Parameters::Parameters(int nargs, const char **vargs, bool jump_first) : Parameters()
    {
        get_params_from_args(nargs, vargs, jump_first);
    }

    void Parameters::get_params_from_args(int nargs, const char **vargs, bool jump_first)
    {
        Arguments args(nargs, vargs);
        //jump first
        if (args.remaining() && jump_first) args.get_string();
        //start
        while (args.remaining())
        {
            bool is_a_valid_arg = false;
            bool parameters_arguments_are_correct = false;
            const char *p = args.get_string();
            for(auto& action : m_params_info)
            {
                if(compare_n_args(action.m_arg_key, p))
                {
                    parameters_arguments_are_correct = action.m_action(args);
                    is_a_valid_arg = true;
                    break;
                } 
            }
            if(!is_a_valid_arg) 
            {
                std::cerr << "parameter " << p << " not found" << std::endl;
                exit(1);
            }
            else if(!parameters_arguments_are_correct) 
            {
                std::cerr << "arguments of parameter " << p << " are not correct" << std::endl;
                exit(1);
            }
        }
    }

    bool Parameters::compare_n_args(const std::vector< std::string >& keys, const char* arg)
    {
        for (const std::string& key : keys) if (strcmp(key.c_str(), arg) == 0) return true;
        return false;
    }

    std::string Parameters::return_n_space(size_t n)
    {
        std::string out;
        while(n--) out+=" ";
        return out;
    }

    std::string Parameters::make_help() const
    {
        std::stringstream s_out;
        //header
        s_out << "Usage: denn [options] <arg>" ;
        s_out << std::endl;				
        s_out << "Options:";				
        s_out << std::endl;
        //style
        const size_t max_space_line         = 60;
        const size_t padding_to_description = 2;
        const size_t padding_left           = 4;
        const std::string separetor(", ");
        //determinate space line init
        size_t space_line_init = 0;
        //
        for(auto& param : m_params_info)
        {
            size_t n_space_used = 0;
            for(auto& key : param.m_arg_key) n_space_used += key.size() + separetor.size();
            space_line_init = std::max(n_space_used, space_line_init);
        }
        //add padding
        space_line_init += padding_to_description;
        std::string padding_left_str = return_n_space(padding_left);
        //print
        for(auto& param : m_params_info)
        {
            size_t space_line = space_line_init;
            s_out << padding_left_str;
            for(auto& key : param.m_arg_key)
            {
                s_out << key << separetor;
                space_line -= key.size() + separetor.size();
            }
            s_out << return_n_space(Denn::clamp<size_t>(space_line,0,max_space_line));
            s_out << param.m_description;
			//show choises
			if (param.m_domain.m_type == ParameterDomain::CHOISE)
			{
				s_out << Dump::json_array<std::string>(param.m_domain.m_choises) << "\n";
			}
			else
			{
				s_out << std::endl;
			}
        }
        s_out << std::endl;
        return s_out.str();
    }
	    
    std::string Parameters::make_help_json() const
    {
        std::stringstream s_out;
        std::string arg_space("  ");
        std::string in_arg_inspace("  ");
        s_out << "[\n";
        for(size_t i = 0; i != m_params_info.size(); ++i)
        {
            auto& param = m_params_info[i];
            if(!param.m_associated_variable) continue;
            
            if(i) s_out << ",\n";
            s_out << arg_space << "{\n";
            s_out << arg_space << in_arg_inspace << "\"name\"" << " : "	<< Dump::json_string(param.m_associated_variable->name()) << ",\n";
            s_out << arg_space << in_arg_inspace << "\"args:\"" << " : " << Dump::json_array<std::string>(param.m_arg_key) << ",\n";
			s_out << arg_space << in_arg_inspace << "\"description\"" << " : " << Dump::json_string(param.m_description) << ",\n";
			s_out << arg_space << in_arg_inspace << "\"serializable\"" << " : " << Dump::json_bool(param.serializable()) << ",\n";
            s_out << arg_space << in_arg_inspace << "\"domain\"" << " : ";
            switch (param.m_domain.m_type)
            {
                case ParameterDomain::MONO:
                    s_out << Dump::json_string(param.m_domain.m_domain) << "\n";
                break;
                case ParameterDomain::CHOISE:
                    s_out << Dump::json_array<std::string>(param.m_domain.m_choises) << "\n";
                break;
                default:
                    s_out << Dump::json_null() << "\n";
                break;
            }
            s_out << arg_space << "}";
        }
        s_out << "\n]";
        s_out << std::endl;
        return s_out.str();
    }
}
