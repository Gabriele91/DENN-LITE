#include "DennParameters.h"
#include <string>
#include <cctype>
#include <unordered_map>

namespace Denn
{
    //////////////////////////////////////////////////////////////////////////////////
    static inline bool conf_is_digit(char c)
    {
        return (c >= '0' && c <= '9');
    }    
    
    static inline bool conf_is_variable(char c)
    {
        return c == '$';
    }

    static inline bool conf_is_xdigit(char c)
    {
        return (c >= '0' && c <= '9') || ((c & ~' ') >= 'A' && (c & ~' ') <= 'F');
    }

    static inline bool conf_is_space(char c)
    {
        return c == ' ' || (c >= '\t' && c <= '\r');
    }    
    
    static inline int conf_char_to_int(char c)
    {
        return (c <= '9') ? c - '0' : (c & ~' ') - 'A' + 10;
    }
    
    static long conf_string_to_int(const char*& source)
    {
        //+/-
        char ch = *source;
        if (ch == '-') ++source;
        //integer part
        long result = 0;
        while (conf_is_digit(*source)) result = (result * 10) + (*source++ - '0');
        //result
        return ch == '-' ? -result : result;
    }
    static long conf_string_to_int_no_skip(const char* source)
    {
        //+/-
        char ch = *source;
        if (ch == '-') ++source;
        //integer part
        long result = 0;
        while (conf_is_digit(*source)) result = (result * 10) + (*source++ - '0');
        //result
        return ch == '-' ? -result : result;
    }
    //////////////////////////////////////////////////////////////////////////////////        
    static bool conf_skip_line_comment(size_t& line, const char*& inout)
    {
        //not a line comment
        if ((*inout) != '/' || *(inout+1) != '/') return false;
        //skeep
        while (*(inout) != EOF && *(inout) != '\0'&& *(inout) != '\n') ++(inout);
        //jump endline
        if ((*(inout)) == '\n')
        {
            ++line;  ++inout;
        }
        //ok
        return true;
    }
    static bool conf_skip_multilines_comment(size_t& line, const char*& inout)
    {
        //not a multilines comment
        if ((*inout) != '/' || *(inout + 1) != '*') return false;
        //jump
        while 
        (
            *(inout) != EOF &&
            *(inout) != '\0' && 
                ((*inout) != '*' || *(inout + 1) != '/')
        )
        {
            line += (*(inout)) == '\n';
            ++inout;
        }
        //jmp last one
        if ((*inout) == '*' && *(inout + 1) == '/') inout += 2;
        //ok
        return true;
    }
    static bool conf_skip_space(size_t& line, const char*& source)
    {
        bool a_space_is_skipped = false;
        while (conf_is_space(*source))
        {
            //to true
            a_space_is_skipped = true;
            //count line
            if (*source == '\n') ++line;
            //jump
            ++source;
            //exit
            if (!*source) break;
        }
        return a_space_is_skipped;
    }    
    static bool conf_skip_space_and_comments(size_t& line, const char*& source)
    {
        bool eat = false;
        while (conf_skip_space(line, source)
            || conf_skip_line_comment(line, source)
            || conf_skip_multilines_comment(line, source)) eat = true;
        return eat;
    }
    static bool conf_skip_line_space(const char*& source)
    {
        bool a_space_is_skipped = false;
        while (conf_is_space(*source) && *source != '\n')
        {
            //to true
            a_space_is_skipped = true;
            //jump
            ++source;
            //exit
            if (!*source) break;
        }
        return a_space_is_skipped;
    }
    static bool conf_skip_line_comment(const char*& inout)
    {
        //not a line comment
        if ((*inout) != '/' || *(inout + 1) != '/') return false;
        //skeep
        while (*(inout) != EOF && *(inout) != '\0'&& *(inout) != '\n') ++(inout);
        //jump endline
        if ((*(inout)) == '\n') return true;
        //ok
        return true;
    }
    static bool conf_skip_line_space_and_comments(size_t& line, const char*& source)
    {
        bool eat = false;
        while (conf_skip_line_space(source)
            || conf_skip_line_comment(source)
            || conf_skip_multilines_comment(line, source)) eat = true;
        return eat;
    }
    //////////////////////////////////////////////////////////////////////////////////        
    static bool conf_rev_skip_line_comment(size_t& line, const char* start, const char*& inout)
    {
        //copy
        const char* in = inout;
        //stat with endline
        if ((*(in)) != '\n') return false;
        //came back to //
        while (in != start && ((*in) != '/' && *(in-1) != '/')) --in;
        //jump //
        if ((*in) != '/' || *(in-1) != '/')
        {
            //jmp //
            inout =  (in - 1) == (start) 
                    ? in - 1
                    : in - 2;
            //remove line
            --line;
            //ok
            return true;
        }
        return false;
    }
    static bool conf_rev_skip_multilines_comment(size_t& line, const char* start, const char*& inout)
    {
        //copy
        const char* in = inout;
        size_t tmp_line = line;
        //test
        if(in == start) return false;
        //not a multilines comment
        if ((*in) != '/' || *(in - 1) != '*') return false;
        //jump
        while 
        (in != start && ((*in) != '*' || *(in - 1) != '/'))
        {
            tmp_line -= (*(in)) == '\n';
            --in;
        }
        //is /*
        if ((*in) == '*' && *(in - 1) == '/')
        {
            //jmp /*
            inout =  (in - 1) == (start) 
                    ? in - 1
                    : in - 2;
            //update line count
            line += tmp_line;
            //ok
            return true;
        }
        return false;
    }
    static bool conf_rev_skip_space(size_t& line, const char* start, const char*& source)
    {
        bool a_space_is_skipped = false;
        while (conf_is_space(*source))
        {
            //to true
            a_space_is_skipped = true;
            //count line
            if (*source == '\n') --line;
            //jump
            --source;
            //exit
            if (source != start) break;
        }
        return a_space_is_skipped;
    }    
    static void conf_rev_skip_space_and_comments(size_t& line, const char* start, const char*& source)
    {
        while (conf_rev_skip_space(line, start, source)
            || conf_rev_skip_line_comment(line, start, source)
            || conf_rev_skip_multilines_comment(line, start, source))
        {
            if(source == start) break;
        }
    }
    static bool conf_rev_skip_line_space(const char* start, const char*& source)
    {
        bool a_space_is_skipped = false;
        while (conf_is_space(*source) && *source != '\n')
        {
            //to true
            a_space_is_skipped = true;
            //jump
            --source;
            //exit
            if (source != start) break;
        }
        return a_space_is_skipped;
    }
    static bool conf_rev_skip_line_comment(const char* start, const char*& inout)
    {
        //copy
        const char* in = inout;
        //stat with endline
        if ((*(in)) != '\n') return false;
        //came back to //
        while (in != start && ((*in) != '/' && *(in - 1) != '/')) --in;
        //jump //
        if ((*in) != '/' || *(in - 1) != '/')
        {
            //jmp //
            inout = (in - 1) == (start)
                ? in - 1
                : in - 2;
            //ok
            return true;
        }
        return false;
    }
    static bool conf_rev_skip_line_space_and_comments(size_t& line, const char* start, const char*& source)
    {
        bool eat = false;
        while (conf_rev_skip_line_space(start, source)
            || conf_rev_skip_line_comment(start, source)
            || conf_rev_skip_multilines_comment(line, start, source))
        {
            eat = true;
            if (source == start) break;
        } 
        return eat;
    }
    //////////////////////////////////////////////////////////////////////////////////        
    struct conf_string_out { std::string m_str; bool m_success; };
    static conf_string_out conf_string(size_t& line, const char*& source)
    {
        //init
        std::string out;
        //start parse
        if ((*source) == '\"')  //["...."]
        {
            ++source;  //[...."]
            while ((*source) != '\"' && (*source) != '\n')
            {
                if ((*source) == '\\') //[\.]
                {
                    ++source;  //[.]
                    switch (*source)
                    {
                    case 'n':
                        out += '\n';
                        break;
                    case 't':
                        out += '\t';
                        break;
                    case 'b':
                        out += '\b';
                        break;
                    case 'r':
                        out += '\r';
                        break;
                    case 'f':
                        out += '\f';
                        break;
                    case 'a':
                        out += '\a';
                        break;
                    case '\\':
                        out += '\\';
                        break;
                    case '?':
                        out += '\?';
                        break;
                    case '\'':
                        out += '\'';
                        break;
                    case '\"':
                        out += '\"';
                        break;
                    case 'u':
                    {
                        int c = 0;
                        //comput u
                        for (int i = 0; i < 4; ++i)
                        {
                            if (conf_is_xdigit(*++source))
                            {
                                c = c * 16 + conf_char_to_int(*source);
                            }
                            else
                            {
                                return { out, false };
                            }
                        }
                        //ascii
                        if (c < 0x80)
                        {
                            out += c;
                        }
                        //utf 2 byte
                        else if (c < 0x800)
                        {
                            out += 0xC0 | (c >> 6);
                            out += 0x80 | (c & 0x3F);
                        }
                        //utf 3 byte
                        else
                        {
                            out += 0xE0 | (c >> 12);
                            out += 0x80 | ((c >> 6) & 0x3F);
                            out += 0x80 | (c & 0x3F);
                        }
                    }
                    break;
                    case '\n': /* jump unix */
                        ++line;
                        break;
                    case '\r': /* jump mac */
                        ++line;
                        if ((*(source + 1)) == '\n') ++source; /* jump window (\r\n)*/
                        break;
                    default:
                        return { out, false };
                        break;
                    }
                }
                else
                {
                    if ((*source) != '\0') out += (*source);
                    else return { out, false };
                }
                ++source;//next char
            }
            //exit cases
            if ((*source) == '\n')
            {
                return { out, false };
            }
            else if ((*source) == '\"')
            {
                ++source;
                return { out, true };
            }
        }
        return { out, false };
    }
    
    static std::string conf_name(const char*& source)
    {
        //init
        std::string out;
        //parse name
        while (
               std::isalpha(*source) 
            || *source == '_' 
            || *source == '-'
        )
        {
            out += (*source);
            ++source;
        }
        //end
        return out;
    }
    ////////////////////////////////////////////////////////////////////////////////////////
    class VariableTable
    {
    public:
        
        void add_vairable(const std::string& variable,const std::string& default_value) const
        {
            m_map[variable] = default_value;
        }
        
        bool change_value(const std::string& variable,const std::string& new_value)
        {
            if(m_map.find(variable) == m_map.end()) return false;       
            m_map[variable] = new_value;
            return true;
        }

        bool exists(const std::string& variable) const
        {
            return m_map.find(variable) != m_map.end();
        }

        std::string get(const std::string& variable) const 
        {
            auto value_it = m_map.find(variable);
            if(value_it == m_map.end()) return "";
            return value_it->second;
        }

        void clear()
        {
            m_map.clear();
        }

    private:

        mutable std::unordered_map< std::string, std::string > m_map;
    };

    class ParametersParseHelp
    {
        public:
            //////////////////////////////////////////////////////////////////////////////////
            static bool conf_parse_arg
            (
                  const std::vector< ParameterInfo >& info
                , const VariableTable& context
                , size_t& line
                , const char*& ptr
                , ParameterInfo ower = ParameterInfo()
            )
            {
                //it's close
                if((*ptr) == '}') return true;
                //command
                std::string command = conf_name(ptr);
                //test
                if (!command.size())
                {
                    std::cerr << line << ": command not valid" << std::endl;
                    return false;
                }
                //jump
                conf_skip_space_and_comments(line, ptr);
                //action
                bool is_a_valid_arg = false;
                //vals            
                for (auto& action : info)
                {
                    //test
                    if (!action.m_associated_variable) continue;
                    if (!action.m_oweners.test(ower)) continue;
                    //search param 
                    bool found = false;
                    //as shell arg or command
                    if (command[0] == '-')
                    {
                        for (const std::string& key : action.m_arg_key)
                        {
                            if (key == command)
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        found = command == action.m_associated_variable->name();
                    }
                    //parse a line 
                    if (found)
                    {
                        //preprocess info
                        struct 
                        {
                            bool        m_success { true };
                            std::string m_name    { ""   };
                        }
                        preprocess_error;
                        //parse argument
                        StringArguments args
                        (
                              [&line](StringArguments& self, const char*& skip_ptr)
                              {
                                  //jump space
                                  conf_skip_line_space_and_comments(line, skip_ptr);
                              }
                            , [&line](StringArguments& self, const char* start,const char*& skip_ptr)
                              {
                                  conf_rev_skip_line_space_and_comments(line, start, skip_ptr);
                              }
                            , [&context, &preprocess_error](std::string& buffer)
                              {
                                  if(buffer.size() && conf_is_variable(buffer[0]))
                                  {
                                      //varname
                                      std::string varname = &buffer[1];
                                      //find
                                      if(!context.exists(varname))
                                      {
                                          preprocess_error.m_success = false;
                                          preprocess_error.m_name = varname;
                                          return;
                                      }
                                      buffer = context.get(varname);
                                  }
                              }
                            , ptr
                            , { '\n', '{', '}' }
                        );
                        //test action
                        if (!action.m_action(args))
                        {
                            //preprocess & fail arg parse?
                            if (!preprocess_error.m_success)
                                std::cerr << line << ": \'" << preprocess_error.m_name << "\' is not valid variable" << std::endl;
                            else
                                std::cerr << line << ": not valid arguments for command \'" << command << "\'" << std::endl;
                            return false;
                        }
                        //preprocess error
                        if (!preprocess_error.m_success)
                        {
                            std::cerr << line << ": \'" << preprocess_error.m_name << "\' is not valid variable" << std::endl;
                            return false;
                        }
                        //update
                        ptr = args.get_ptr();
                        conf_skip_space_and_comments(line, ptr);
                        //sub args?
                        if (*ptr == '{')
                        {
                            //jump {
                            ++ptr;
                            conf_skip_space_and_comments(line, ptr);
                            //sub args
                            do
                            {
                                if (!conf_parse_arg(info, context, line, ptr, action)) return false;
                            } 
                            while (*ptr && (*ptr) != '}');
                            //test
                            if (*ptr != '}')
                            {
                                std::cerr << line << ": } not found" << std::endl;
                                return false;
                            }
                            //jump }
                            ++ptr;
                            conf_skip_space_and_comments(line, ptr);
                        }
                        //ok
                        is_a_valid_arg = true;
                        break;
                    }
                }
                //fail
                if (!is_a_valid_arg)
                {
                    std::cerr << line << ": command \'" << command << "\' not found" << std::endl;
                    return false;
                }
                //jump
                conf_skip_space_and_comments(line, ptr);
                return true;
            }
            //////////////////////////////////////////////////////////////////////////////////
            static bool conf_parse_net
            (
                  Parameters& params
                , const VariableTable& context
                , size_t& line
                , const char*& ptr
            )
            {
                //it's close
                if((*ptr) == '}') return true;
                //layer type
                std::string layer_type = conf_name(ptr);
                //type
                if (layer_type == "lp" || layer_type == "layer_perceptron")
                {
                    //jump space
                    conf_skip_line_space_and_comments(line, ptr);
                    //get int
                    long layer_size;
                    //is not a variable?
                    if(!conf_is_variable(*ptr))
                    {
                        layer_size = conf_string_to_int(ptr);
                    }
                    else
                    {
                        //next
                        ++ptr;
                        //varname
                        std::string varname = conf_name(ptr);
                        //find
                        if(!context.exists(varname))
                        {
                            std::cerr << line << ": \'" << varname << "\' is not valid variable" << std::endl;
                            return false;
                        }
                        //value
                        std::string value = context.get(varname);
                        //parse
                        layer_size = conf_string_to_int_no_skip(value.c_str());
                    }
                    //more than 0
                    if (layer_size <= 0)
                    {
                        std::cerr << line << ": layer size (" << layer_size << ") not valid " << std::endl;
                        return false;
                    }
                    //jump space
                    conf_skip_line_space_and_comments(line, ptr);
                    //activation function (default linear)
                    std::string layer_af;
                    //is not a variable?
                    if(!conf_is_variable(*ptr))
                    {
                        layer_af = conf_name(ptr);
                    }
                    else
                    {
                        //next
                        ++ptr;
                        //varname
                        std::string varname = conf_name(ptr);
                        //find
                        if(!context.exists(varname))
                        {
                            std::cerr << line << ": \'" << varname << "\' is not valid variable" << std::endl;
                            return false;
                        }
                        //parse
                        layer_af = context.get(varname);
                    }
                    //test
                    if (!layer_af.size()) layer_af = "linear";
                    //test
                    if (!ActivationFunctionFactory::exists(layer_af))
                    {
                        std::cerr << line << ": activation function (" << layer_af << ") not exists " << std::endl;
                        return false;
                    }
                    //add
                    params.m_hidden_layers.get().push_back(layer_size);
                    params.m_activation_functions.get().push_back(layer_af);
                }
                else if (layer_type == "out" || layer_type == "output")
                {
                    //jump space
                    conf_skip_line_space_and_comments(line, ptr);
                    //activation function (default linear)
                    std::string layer_af;
                    //is not a variable?
                    if(!conf_is_variable(*ptr))
                    {
                        layer_af = conf_name(ptr);
                    }
                    else
                    {
                        //next
                        ++ptr;
                        //varname
                        std::string varname = conf_name(ptr);
                        //find
                        if(!context.exists(varname))
                        {
                            std::cerr << line << ": \'" << varname << "\' is not valid variable" << std::endl;
                            return false;
                        }
                        //parse
                        layer_af = context.get(varname);
                    }
                    //test
                    if (!layer_af.size()) layer_af = "linear";
                    //test
                    if (!ActivationFunctionFactory::exists(layer_af))
                    {
                        std::cerr << line << ": activation function (" << layer_af << ") not exists " << std::endl;
                        return false;
                    }
                    //add
                    params.m_output_activation_function = layer_af;
                }
                else
                {
                    std::cerr << line << ": layer " << layer_type << " is unsupported" << std::endl;
                    return false;
                }
                //jump spaces
                conf_skip_space_and_comments(line, ptr);
                return true;
            }
            //////////////////////////////////////////////////////////////////////////////////
            static bool conf_parse_variable
            (
                  const Parameters& params
                , VariableTable& context
                , size_t& line
                , const char*& ptr
            )
            {
                //it's close
                if((*ptr) == '}') return true;
                //variable type
                std::string variable_name = conf_name(ptr);
                //jump spaces
                conf_skip_space_and_comments(line, ptr);
                //value
                std::string value;
                //endline?
                while(*ptr && (*ptr)!='\n') value += *(ptr++);
                //set
                if(!context.exists(variable_name))
                {
                    context.add_vairable(variable_name, value);
                }
                //jump spaces
                conf_skip_space_and_comments(line, ptr);
                return true;
            }
    };
    //////////////////////////////////////////////////////////////////////////////////
    bool Parameters::from_config(const std::string& source, int nargs, const char **vargs)
    {
        //ptr
        const char* ptr = source.c_str();
        size_t line = 0;
        //jump
        conf_skip_space_and_comments(line, ptr);
        //Var table
        VariableTable context;
        //parse variable from comand line
        for(int var = 0; (var+2) < nargs ; var+=3)
        {
            //name
            std::string name = vargs[var];
            //=
            if(vargs[var+1][0] != '=') break;
            //value
            std::string value = vargs[var+2];
            //add var
            context.add_vairable(name, value);
        }
        //parsing
        while (*ptr)
        {
            //command
            std::string command = conf_name(ptr);
            //test
            if (!command.size())
            {
                std::cerr << line << ": command not valid" << std::endl;
                return false;
            }
            //find '{'
            conf_skip_space_and_comments(line, ptr);
            //test
            if (*ptr != '{')
            {
                std::cerr << line << ": { not found" << std::endl;
                return false;
            }
            //jump {
            ++ptr;
            conf_skip_space_and_comments(line, ptr);
            //type
            if (command == "variable")
            {
                do
                {
                    if (!ParametersParseHelp::conf_parse_variable(*this, context, line, ptr)) return false;
                }
                while (*ptr && (*ptr) != '}');
            }
            else if (command == "network")
            {
                do
                {
                    if (!ParametersParseHelp::conf_parse_net(*this, context, line, ptr)) return false;
                }
                while (*ptr && (*ptr) != '}');
            }
            else if (command == "args")
            {
                do
                {
                    if (!ParametersParseHelp::conf_parse_arg(m_params_info, context, line, ptr)) return false;
                }
                while (*ptr && (*ptr) != '}');
            }
            //test
            if (*ptr != '}')
            {
                std::cerr << line << ": } not found" << std::endl;
                return false;
            }
            //jump }
            ++ptr;
            conf_skip_space_and_comments(line, ptr);
            //loop
        }
        return true;
    }
}