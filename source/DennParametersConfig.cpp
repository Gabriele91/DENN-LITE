#include "DennParameters.h"
#include "DennActivationFunction.h"
#include <string>
#include <cctype>
#include <unordered_map>
#include <iterator>
#include <sstream>

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
	protected:

		class ConfArguments : public Arguments
		{
		public:
			//alias
			using ListChar = std::vector < char >;
			using ListString = std::vector < std::string >;
			//class methods
			ConfArguments(const VariableTable& table, size_t& line, const char* ptr,const ListChar& end_line)
			{
				//init 
				m_ptr = ptr;
				m_index = 0;
				//parsing
				while (*m_ptr && std::find(end_line.begin(), end_line.end(), *m_ptr) == end_line.end())
				{
					//value
					std::string value;
					//remove space
					conf_skip_line_space_and_comments(line, m_ptr);
					//get value
					while (!std::isspace(*m_ptr)) value += *(m_ptr++);
					//push in list
					variable_processing(table,line,value);
				}
			}

			const char* get_string() override
			{
				denn_assert(!eof());
				//get
				return m_values[m_index++].c_str();
			}

			bool get_bool() override
			{
				std::string arg = get_string();
				std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
				return arg == std::string("true")
					|| arg == std::string("yes")
					|| arg == std::string("t")
					|| arg == std::string("y");
			}

			int get_int() override
			{
				return atoi(get_string());
			}

			double get_double() override
			{
				return atof(get_string());
			}

			bool eof() const override
			{
				return m_values.size() <= m_index;
			}

			bool back() override
			{
				//index is 0? It can't go back
				if (!m_index) return false;
				//else go back
				--m_index;
				//ok
				return true;
			}

			bool end_vals() const override
			{
				return eof();
			}

			const ListString& errors() const
			{
				return m_errors;
			}

			const char* get_ptr() const
			{
				return m_ptr;
			}

		protected:

			//ptr
			const char* m_ptr;
			//info buffer
			size_t m_index;
			ListString m_values;
			//errors
			ListString m_errors;
			//variable processing
			bool variable_processing(const VariableTable& context, const size_t& line, const std::string& input) 
			{
				if (input.size() && conf_is_variable(input[0]))
				{
					//varname
					std::string varname = &input[1];
					//find
					if (!context.exists(varname))
					{
						m_errors.push_back(line + ": \'" + varname + "\' is not valid variable");
						return false;
					}
					//get value
					std::stringstream end_value ( context.get(varname) );
					//split by space
					std::istream_iterator<std::string> it(end_value);
					std::istream_iterator<std::string> end;
					//add all tokens
					for (; it != end; ++it) m_values.push_back(*it);
				}
				else
				{
					//add input
					m_values.push_back(input);
				}
			}
		};

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
					//line parser
					ConfArguments args(context, line, ptr, { '\n', '}', '{' });
                    //test action
                    if (!action.m_action(args))
                    {
                        //preprocess & fail arg parse?
                        if (args.errors().size())
							for (const auto& error : args.errors()) std::cerr << error << std::endl;
                        else
                            std::cerr << line << ": not valid arguments for command \'" << command << "\'" << std::endl;
                        return false;
                    }
                    //preprocess error
                    if (args.errors().size())
                    {
						for(const auto& error : args.errors()) std::cerr << error << std::endl;
                        return false;
                    }
                    //update
                    ptr = args.get_ptr();
					//next value
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
		//////////////////////////////////////////////////////////////////////////////////
		static bool conf_parse_cline_args
		(
			  VariableTable& context
			, int nargs
			, const char **vargs
		)
		{
			enum State
			{
			  START
			, NAME
			, EQUAL
			, VALUE
			};
			//Parse state
			State state{ START };
			//values
			std::string name;
			std::string value;
			//parsing
			for (int i = 0; i < nargs; ++i)
			{
				//ptr
				const char* ptr = vargs[i];
				//parse
				while (*ptr)
				{
					switch (state)
					{
					case START:
						//begin
						conf_skip_line_space(ptr);
						name.clear();
						value.clear();
						state = NAME;
					break;
					case NAME:
						name = conf_name(ptr);
						if (!name.size())
						{
							std::cerr << "Name argument is not valid" << std::endl;
							return false;
						}
						state = EQUAL;
					break;
					case EQUAL:
						if (*ptr != '=')
						{
							std::cerr << "\'=\' is not found" << std::endl;
							return false;
						}
						++ptr;
						state = VALUE;
					break;
					case VALUE:
						while(*ptr) value += (*ptr++);
						if (!value.size())
						{
							std::cerr << "Value argument is not valid" << std::endl;
							return false;
						}
						state = START;
						//add value
						context.add_vairable(name, value);
					break;
					default:
						return false;
					break;
					}
				}

			}
			//parsing end state
			return state == START;
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
        //parse variables from comand line
		if (!ParametersParseHelp::conf_parse_cline_args(context, nargs, vargs))
			return false;
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