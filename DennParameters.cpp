#include "DennParameters.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennEvolutionMethod.h"

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


    Parameters::Parameters() 
    :m_params_info
    ({
        ParameterInfo{ 
            "Global number of generation", { "--generation",    "-t"  }, 
            [this](Arguments& args) -> bool { m_generations = args.get_int() ; return true; } 
        },
        ParameterInfo{ 
            "Number of generation per batch", { "--sub_gens",    "-s"  }, 
            [this](Arguments& args) -> bool { m_sub_gens = args.get_int() ; return true; }
        },
        ParameterInfo{
            "Number of parents", { "--number_parents",    "-np"  }, 
            [this](Arguments& args) -> bool { m_np = args.get_int() ; return true; }
        },
        ParameterInfo{
            "Type of evolution method [" + EvolutionMethodFactory::names_of_evolution_methods() + "]", { "--evolution_method", "--evolution",    "-em"  },
            [this](Arguments& args) -> bool  
            { 
                std::string str_m_type = args.get_string() ;
                //all lower case
                std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::tolower);
                //save
				m_evolution_type = str_m_type;
                //ok
                return MutationFactory::exists(*m_mutation_type); 
            }
        },
        ParameterInfo{
            "Type of DE mutation [" + MutationFactory::names_of_mutations() + "]", { "--mutation",    "-m"  },
            [this](Arguments& args) -> bool  
            { 
                std::string str_m_type = args.get_string() ;
                //all lower case
                std::transform(str_m_type.begin(),str_m_type.end(), str_m_type.begin(), ::tolower);
                //save
                m_mutation_type = str_m_type;
                //ok
                return MutationFactory::exists(*m_mutation_type); 
            }
        },
        ParameterInfo{
            "Type of DE crossover [" + CrossoverFactory::names_of_crossovers() + "]", { "--crossover",    "-co"  },
            [this](Arguments& args) -> bool 
            { 
                std::string str_c_type = args.get_string();
                //all lower case
                std::transform(str_c_type.begin(),str_c_type.end(), str_c_type.begin(), ::tolower);
                //save
                m_crossover_type = str_c_type;
                //ok
                return CrossoverFactory::exists(*m_crossover_type); 
            }
        },

        ParameterInfo{ 
            "Default F factor for DE", { "--f_default",    "-f"  }, 
            [this](Arguments& args) -> bool { m_default_f = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            "Default CR factor for DE", { "--cr_default",    "-cr"  }, 
            [this](Arguments& args) -> bool { m_default_cr = args.get_double() ;  return true; }
        },
                            
        ParameterInfo{ 
            "Probability of change of F (JDE)", { "--f_jde",    "-jf"  }, 
            [this](Arguments& args) -> bool { m_jde_f = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            "Probability of change of CR (JDE)", { "--cr_jde",    "-jcr"  }, 
            [this](Arguments& args) -> bool { m_jde_cr = args.get_double() ;  return true; } 
        },
				
		ParameterInfo{
			"Archive size (JADE)",{ "--archive_size",    "-as" },
			[this](Arguments& args) -> bool { m_archive_size = args.get_int();  return true; }
		},				
		ParameterInfo{
			"Auto adaptation factor of f and c parameters (JADE)",{ "--adapt_f_cr",    "-afcr" },
			[this](Arguments& args) -> bool { m_f_cr_adapt = args.get_double();  return true; }
		},		

		ParameterInfo{
			"Percentage of best (Current P Best/JADE)",{ "--perc_of_best",    "-pob" },
			[this](Arguments& args) -> bool { m_perc_of_best = args.get_double();  return true; }
		},

        ParameterInfo{ 
            "Minimum size of weight", { "--clamp_min",    "-cmin"  }, 
            [this](Arguments& args) -> bool { m_clamp_min = args.get_double() ; return true;  } 
        },
        ParameterInfo{ 
            "Maximum size of weight", { "--clamp_min",    "-cmax"  }, 
            [this](Arguments& args) -> bool { m_clamp_max = args.get_double() ; return true;  } 
        },
        ParameterInfo{ 
            "Minimum size of weight in random initialization", { "--random_min",    "-rmin"  }, 
            [this](Arguments& args) -> bool { m_range_min = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            "Maximum size of weight in random initialization", { "--random_max",    "-rmax"  }, 
            [this](Arguments& args) -> bool { m_range_max = args.get_double() ;  return true; } 
        },
        ParameterInfo{ 
            "Number of change of batches before restart (if accuracy not increase)", { "--restart_count",    "-rc"  }, 
            [this](Arguments& args) -> bool  
            { 
                m_restart_count = args.get_int() ; 
                if(*m_restart_count < 0) m_restart_enable = false; 
                return true; 
            } 
        },
        ParameterInfo{ 
            "Delta factor to determine if accuracy is increased", { "--restart_delta",    "-rd"  }, 
            [this](Arguments& args) -> bool 
            {
                m_restart_delta = args.get_double() ; 
                if(*m_restart_delta<0) m_restart_enable = false;
                 return true; 
            } 
        },
        ParameterInfo{
            "Size of hidden layers", { "--hidden_layers",    "-hl"  },
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
            "Path of dataset file (gz)", { "--dataset", "-d", "-i" }, 
            [this](Arguments& args) -> bool {  m_dataset_filename = args.get_string();  return true; } 
        },
        ParameterInfo{ 
            "Path of output file (json)", { "--output", "-o" }, 
            [this](Arguments& args) {  m_output_filename = args.get_string(); return true;  } 
        },
        ParameterInfo{ 
            "Number of threads using by OpenMP", { "--threads_omp",    "-omp"  }, 
            [this](Arguments& args) -> bool { m_threads_omp = args.get_int() ;  return true; } 
        },
        ParameterInfo{ 
            "Number of threads using for  generate a new population", { "--threads_pop",    "-tp"  }, 
            [this](Arguments& args) -> bool { m_threads_pop = args.get_int() ;  return true; } 
        },
        ParameterInfo{
            "Print list of muation", { "--mutation-list",    "-mlist"  }, 
            [this](Arguments& args) -> bool { std::cout << MutationFactory::names_of_mutations() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print list of crossover", { "--crossover-list",    "-colist"  }, 
            [this](Arguments& args) -> bool { std::cout << CrossoverFactory::names_of_crossovers() << std::endl; return true; } 
        },
        ParameterInfo{
            "Print the help", { "--help",    "-h"  }, 
            [this](Arguments& args) -> bool { std::cout << make_help();  return true; } 
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
        const size_t max_space_line         = 30;
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

}