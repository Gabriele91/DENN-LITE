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
            m_generations, "Global number of generation", { "--" + m_generations.name(),    "-t"  }, 
            [this](Arguments& args) -> bool { m_generations = args.get_int() ; return true; } 
        },
        ParameterInfo{ 
            m_sub_gens, "Number of generation per batch", { "--sub_gens" + m_sub_gens.name(),    "-s"  }, 
            [this](Arguments& args) -> bool { m_sub_gens = args.get_int() ; return true; }
        },
        ParameterInfo{
            m_np, "Number of parents", { "--" + m_np.name(),    "-np"  }, 
            [this](Arguments& args) -> bool { m_np = args.get_int() ; return true; }
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
        },
        ParameterInfo{
            m_mutation_type, "Type of DE mutation [" + MutationFactory::names_of_mutations() + "]", { "--" + m_mutation_type.name(),    "-m"  },
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
            m_crossover_type, "Type of DE crossover [" + CrossoverFactory::names_of_crossovers() + "]", { "--" + m_crossover_type.name(),    "-co"  },
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
			m_archive_size, "Archive size (JADE)",{ "--" + m_archive_size.name(),    "-as" },
			[this](Arguments& args) -> bool { m_archive_size = args.get_int();  return true; }
		},				
		ParameterInfo{
			m_f_cr_adapt, "Auto adaptation factor of f and c parameters (JADE)",{ "--" + m_f_cr_adapt.name(),    "-afcr" },
			[this](Arguments& args) -> bool { m_f_cr_adapt = args.get_double();  return true; }
		},		

		ParameterInfo{
			m_perc_of_best, "Percentage of best (Current P Best/JADE)",{ "--" + m_perc_of_best.name(),    "-pob" },
			[this](Arguments& args) -> bool { m_perc_of_best = args.get_double();  return true; }
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
            m_dataset_filename, "Path of dataset file (gz)", { "--" + m_dataset_filename.name(), "-d", "-i" }, 
            [this](Arguments& args) -> bool {  m_dataset_filename = args.get_string();  return true; } 
        },
        ParameterInfo{ 
            m_output_filename, "Path of output file (json)", { "--" + m_output_filename.name(), "-o" }, 
            [this](Arguments& args) {  m_output_filename = args.get_string(); return true;  } 
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
        const size_t max_space_line         = 42;
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