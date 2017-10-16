#include "DennParameters.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennActiveFunction.h"
#include "DennEvolutionMethod.h"
#include "DennRuntimeOutput.h"
#include "DennSerializeOutput.h"
#include "DennDump.h"
#include "DennVersion.h"

namespace Denn
{
    Arguments::Arguments(int nargs, const char** vargs)
    : m_rem_arg(nargs)
    , m_pointer(vargs)
    {
    }

    const char* Arguments::get_string()
    {
        assert(m_rem_arg);
        --m_rem_arg;
        return *(m_pointer++);
    }

    int Arguments::get_int() 
    {
        return atoi(get_string());
    }

    double Arguments::get_double()
    {
        return atof(get_string());
    }

    int Arguments::remaining() const
    {
        return m_rem_arg;
    }

    void Arguments::back_of_one()
    {
        ++m_rem_arg;
        --m_pointer;
    }

    bool Arguments::start_with_minus() const
    {
        return (*m_pointer)[0] == '-';
    }

    //CPP type to str param
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<bool>(){ return "bool"; }
    
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<char>()                        { return "string"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< const char* >()               { return "string"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::string >()               { return "string"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector< std::string > >(){ return "list(string)"; }
    
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<short>()    { return "int"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<int>()      { return "int"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<long>()     { return "int"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<long long>(){ return "int"; }
    
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<unsigned short>()    { return "uint"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<unsigned int>()      { return "uint"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<unsigned long>()     { return "uint"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<unsigned long long>(){ return "uint"; }
    
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<float>()      { return "float"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<double>()     { return "float"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<long double>(){ return "float"; }
    
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<MatrixF>() { return "matrix"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<MatrixD>() { return "matrix"; }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type<MatrixLD>(){ return "matrix"; }

	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<short> >()		  { return "list(int)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<int> >()		  { return "list(int)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<long> >()		  { return "list(int)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<long long> >()	  { return "list(int)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned short> >()		  { return "list(uint)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned int> >()		  { return "list(uint)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long> >()		  { return "list(uint)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<unsigned long long> >()	  { return "list(uint)"; }
	template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<float> >()        { return "list(float)";  }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<double> >()       { return "list(float)";  }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<long double> >()  { return "list(float)";  }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixF> >()      { return "list(matrix)";  }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixD> >()      { return "list(matrix)";  }
    template<> const char* Parameters::ParameterInfo::cpp_type_to_arg_type< std::vector<MatrixLD> >()     { return "list(matrix)";  }
    
    
    Parameters::Parameters() 
    :m_params_info
    ({
        ParameterInfo{ 
            m_generations, "Global number of generation [or backpropagation pass]", { "--" + m_generations.name(), "-t", "-g"  }, 
            [this](Arguments& args) -> bool { m_generations = args.get_int() ; return true; } 
        },
        ParameterInfo{ 
            m_sub_gens, "Number of generation [or backpropagation pass] per batch", { "--" + m_sub_gens.name(), "-s" , "-sg" },
            [this](Arguments& args) -> bool { m_sub_gens = args.get_int() ; return true; }
        },
        ParameterInfo{
            m_np, "Number of parents", { "--" + m_np.name(),    "-np"  }, 
            [this](Arguments& args) -> bool { m_np = args.get_int() ; return true; }
        },
        ParameterInfo{
            m_seed, "Random generator seed", { "--" + m_seed.name(),    "-sd"  }, 
            [this](Arguments& args) -> bool { m_seed = args.get_int() ; return true; }
        },
        ParameterInfo{
            m_batch_size, "Batch size", { "--" + m_batch_size.name(), "-b" },
            [this](Arguments& args) -> bool 
			{
				m_batch_size = args.get_int();
				return true; 
			}
        },
        ParameterInfo{
            m_batch_offset, "Batch offset, how many records will be replaced in the next batch [<= batch size]", { "--" + m_batch_offset.name(), "-bo" },
            [this](Arguments& args) -> bool {  m_batch_offset = args.get_int();  return true; }
        },
        ParameterInfo{
            m_use_validation, "Use the validation test", { "--" + m_use_validation.name(), "-uv" },
            [this](Arguments& args) -> bool
            {
                std::string arg(args.get_string());
                m_use_validation =
                    arg == std::string("true")
                ||  arg == std::string("yes")
                ||  arg == std::string("t")
                ||  arg == std::string("y") ;
                return true;
            }
        },
        ParameterInfo{
            m_evolution_type, "Type of evolution method [" + EvolutionMethodFactory::names_of_evolution_methods() + "]", { "--" + m_evolution_type.name(), "--evolution",    "-em"  },
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
            , "Type of sub evolution method [" + EvolutionMethodFactory::names_of_evolution_methods() + "] (PHISTORY)"
            , { "--" + m_sub_evolution_type.name(), "--sub-evolution",    "-sem"  }
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
            , "Type of DE mutation [" + MutationFactory::names_of_mutations() + "]"
            , { "--" + m_mutation_type.name(),    "-m"  }
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
            , "Type of DE crossover [" + CrossoverFactory::names_of_crossovers() + "]"
            , { "--" + m_crossover_type.name(),    "-co"  }
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
            m_history_size, "Size of population history (PHISTORY)", { "--" + m_history_size.name(),    "-hs"  }, 
            [this](Arguments& args) -> bool { m_history_size = args.get_int() ; return true; }
        },

        ParameterInfo{ 
            m_default_f, "Default F factor for DE", { "--" + m_default_f.name() ,    "-f"  }, 
            [this](Arguments& args) -> bool { m_default_f = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            m_default_cr, "Default CR factor for DE", { "--" + m_default_cr.name() ,    "-cr"  }, 
            [this](Arguments& args) -> bool { m_default_cr = args.get_double() ;  return true; }
        },
                            
        ParameterInfo{ 
            m_jde_f, "Probability of change of F (JDE)", { "--" + m_jde_f.name(),    "-jf"  }, 
            [this](Arguments& args) -> bool { m_jde_f = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            m_jde_cr, "Probability of change of CR (JDE)", { "--" + m_jde_cr.name(),    "-jcr"  }, 
            [this](Arguments& args) -> bool { m_jde_cr = args.get_double() ;  return true; } 
        },
				
		ParameterInfo{
			m_archive_size, "Archive size (JADE/SHADE)",{ "--" + m_archive_size.name(),    "-as" },
			[this](Arguments& args) -> bool { m_archive_size = args.get_int();  return true; }
		},				
		ParameterInfo{
			m_f_cr_adapt, "Auto adaptation factor of f and c parameters (JADE)",{ "--" + m_f_cr_adapt.name(),    "-afcr" },
			[this](Arguments& args) -> bool { m_f_cr_adapt = args.get_double();  return true; }
		},

		ParameterInfo{
			m_shade_h, "Size of archive of mu_f and mu_cr in SHADE",{ "--" + m_shade_h.name(),    "-shah" },
			[this](Arguments& args) -> bool { m_shade_h = args.get_int();  return true; }
		}, 

		ParameterInfo{
			m_min_np, "Minimum number of parents (L-SHADE)",{ "--" + m_min_np.name(),    "-mnp" },
			[this](Arguments& args) -> bool { m_min_np = args.get_int(); return true; }
		},
		ParameterInfo{
				m_max_nfe, "Maximum number of fitness evaluation (L-SHADE)",{ "--" + m_max_nfe.name(),  "-mnfe" },
			[this](Arguments& args) -> bool { m_max_nfe = args.get_int(); return true; }
		},
		ParameterInfo{
			m_mu_cr_terminal_value, "Terminal value of MU Cr (L-SHADE)",{ "--" + m_mu_cr_terminal_value.name(),  "-mcrtv" },
			[this](Arguments& args) -> bool { m_mu_cr_terminal_value = args.get_double(); return true; }
		},

		ParameterInfo{
			m_perc_of_best, "Percentage of best (Current P Best/NO [L-]SHADE)",{ "--" + m_perc_of_best.name(), "-pob", "-p" },
			[this](Arguments& args) -> bool { m_perc_of_best = args.get_double();  return true; }
		},	

		ParameterInfo{
			m_degl_scalar_weight, "Scalar weight used to combinate local mutant with global mutant (DEGL)",{ "--" + m_degl_scalar_weight.name(),    "-glw" },
			[this](Arguments& args) -> bool { m_degl_scalar_weight = args.get_double();  return true; }
		},				
		ParameterInfo{
			m_degl_neighborhood, "Neighborhood of target where is searched the best (DEGL)",{ "--" + m_degl_neighborhood.name(), "-glnn" },
			[this](Arguments& args) -> bool { m_degl_neighborhood = args.get_int();  return true; }
		},	
		ParameterInfo{
			m_trig_m, "Probability of the mutation to be trigonometric (TDE)",{ "--" + m_trig_m.name(), "-tm" },
			[this](Arguments& args) -> bool { m_trig_m = args.get_double();  return true; }
        },
        
        ParameterInfo{ 
            m_clamp_min, "Minimum size of weight", { "--" + m_clamp_min.name(),    "-cmin"  }, 
            [this](Arguments& args) -> bool { m_clamp_min = args.get_double() ; return true;  } 
        },
        ParameterInfo{ 
            m_clamp_max, "Maximum size of weight", { "--" + m_clamp_max.name(),    "-cmax"  }, 
            [this](Arguments& args) -> bool { m_clamp_max = args.get_double() ; return true;  } 
        },
        ParameterInfo{ 
            m_range_min, "Minimum size of weight in random initialization", { "--" + m_range_min.name(),    "-rmin"  }, 
            [this](Arguments& args) -> bool { m_range_min = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            m_range_max, "Maximum size of weight in random initialization", { "--" + m_range_max.name(),    "-rmax"  }, 
            [this](Arguments& args) -> bool { m_range_max = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            m_restart_count, "Number of change of batches before restart (if accuracy not increase)", { "--" + m_restart_count.name(),    "-rc"  }, 
            [this](Arguments& args) -> bool  
            { 
                m_restart_count = args.get_int() ; 
                if(*m_restart_count < 0) m_restart_enable = false; 
                return true; 
            } 
        },
        ParameterInfo{ 
            m_restart_delta, "Delta factor to determine if accuracy is increased", { "--" + m_restart_delta.name(),    "-rd"  }, 
            [this](Arguments& args) -> bool 
            {
                m_restart_delta = args.get_double() ; 
                if(*m_restart_delta<0) m_restart_enable = false;
                 return true; 
            } 
        },
        ParameterInfo{ 
			m_learning_rate, "Learning rate of backpropagation for each pass", { "--" + m_learning_rate.name(),    "-lrbp"  },
            [this](Arguments& args) -> bool 
            {
				m_learning_rate = args.get_double() ;
                return Scalar(0.0) <= *m_learning_rate;
            } 
        },
        ParameterInfo{ 
			m_regularize, "Regularize of backpropagation for each pass", { "--" + m_regularize.name(),    "-rbp"  },
            [this](Arguments& args) -> bool 
            {
				m_regularize = args.get_double() ;
                return Scalar(0.0) <= *m_regularize;
            } 
        },
        ParameterInfo{
            m_hidden_layers, "Size of hidden layers", { "--" + m_hidden_layers.name(),    "-hl"  },
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
            m_active_functions, "Activation functions of hidden layers [" + ActiveFunctionFactory::names_of_active_functions() + "]", { "--" + m_active_functions.name(), "--activation_functions" ,  "-hlaf"  },
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
        },
        ParameterInfo{
			m_output_active_function, "Activation function of output layer [" + ActiveFunctionFactory::names_of_active_functions() + "]", { "--" + m_output_active_function.name(), "--output_activation_functions" ,  "-oaf"  },
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
        }, 
        ParameterInfo{
            m_dataset_filename, "Path of dataset file (gz)", { "--" + m_dataset_filename.name(), "-d", "-i" }, 
            [this](Arguments& args) -> bool {  m_dataset_filename = args.get_string();  return true; } 
        },
        ParameterInfo{ 
            m_output_filename, "Path of output file (json)", { "--" + m_output_filename.name(), "-o" }, 
            [this](Arguments& args) {  m_output_filename = args.get_string(); return true;  } 
        },
        ParameterInfo{
            m_runtime_output_type, "Select type of runtime output [" + RuntimeOutputFactory::names_of_runtime_outputs() + "]", { "--" + m_runtime_output_type.name(),    "-ro"  }, 
            [this](Arguments& args) -> bool { m_runtime_output_type = args.get_string() ; return true; }
        },
        ParameterInfo{
            m_runtime_output_file, "Write the runtime output to stream [::cout, ::cerr, <file>]", { "--" + m_runtime_output_file.name(),    "-rof"  }, 
            [this](Arguments& args) -> bool { m_runtime_output_file = args.get_string() ; return true; }
        },
        ParameterInfo{
            m_compute_test_per_pass, "Compute the test accuracy for each pass", { "--" + m_compute_test_per_pass.name(),    "-ctps"  }, 
            [this](Arguments& args) -> bool 
            { 
                std::string arg(args.get_string());
                m_compute_test_per_pass = 
                    arg == std::string("true") 
                ||  arg == std::string("yes") 
                ||  arg == std::string("t")
                ||  arg == std::string("y") ; 
                return true; 
            }
        },
        ParameterInfo{
            m_serialize_neural_network, "Serialize the neural network of best individual", { "--" + m_serialize_neural_network.name(),    "-snn"  },
            [this](Arguments& args) -> bool 
            { 
                std::string arg(args.get_string());
				m_serialize_neural_network =
                    arg == std::string("true") 
                ||  arg == std::string("yes") 
                ||  arg == std::string("t")
                ||  arg == std::string("y") ; 
                return true; 
            }
        },

        ParameterInfo{ 
            m_threads_omp, "Number of threads using by OpenMP", { "--" + m_threads_omp.name(),    "-omp"  }, 
            [this](Arguments& args) -> bool { m_threads_omp = args.get_int() ;  return true; } 
        },
        ParameterInfo{ 
            m_threads_pop, "Number of threads using for  generate a new population", { "--" + m_threads_pop.name(),    "-tp"  }, 
            [this](Arguments& args) -> bool { m_threads_pop = args.get_int() ;  return true; } 
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
                std::cerr << "parameter" << p << " not found\n";
                exit(1);
            }
            else if(!parameters_arguments_are_correct) 
            {
                std::cerr << "arguments of parameter " << p << " not are correct\n";
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
            s_out << std::endl;
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
