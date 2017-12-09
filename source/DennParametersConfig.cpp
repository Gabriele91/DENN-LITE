#include "DennParameters.h"
#include <cctype>

namespace Denn
{
	//////////////////////////////////////////////////////////////////////////////////
	static inline bool conf_is_digit(char c)
	{
		return (c >= '0' && c <= '9');
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
	//////////////////////////////////////////////////////////////////////////////////
	class ParametersParseHelp
	{
		public:
			//////////////////////////////////////////////////////////////////////////////////
			static bool conf_parse_arg
			(
				  const std::vector< ParameterInfo >& info
				, size_t& line
				, const char*& ptr
				, ParameterInfo ower = ParameterInfo()
			)
			{
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
						//parse argument
						StringArguments args
						(
							  [&line](StringArguments& self, const char*& skip_ptr)
							  {
									while (conf_skip_line_space(skip_ptr)
										|| conf_skip_line_comment(skip_ptr)
										|| conf_skip_multilines_comment(line, skip_ptr));
							  }
							, [&line](StringArguments& self, const char* start,const char*& skip_ptr)
							  {
								  while (conf_rev_skip_line_space(start, skip_ptr)
									  || conf_rev_skip_line_comment(start, skip_ptr)
									  || conf_rev_skip_multilines_comment(line, start, skip_ptr))
								  {
									  if(skip_ptr == start) break;
								  }
							  }
							, ptr
							, { '\n', '{', '}' }
						);
						if (!action.m_action(args))
						{
							std::cerr << line << ": not valid arguments for command \'" << command << "\'" << std::endl;
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
								if (!conf_parse_arg(info, line, ptr, action)) return false;
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
				, size_t& line
				, const char*& ptr
			)
			{
				//layer type
				std::string layer_type = conf_name(ptr);
				//type
				if (layer_type == "lp" || layer_type == "layer_perceptron")
				{
					//jump space
					while (conf_skip_line_space(ptr)
						|| conf_skip_line_comment(ptr)
						|| conf_skip_multilines_comment(line, ptr));
					//get int
					long layer_size = conf_string_to_int(ptr);
					//more than 0
					if (layer_size <= 0)
					{
						std::cerr << line << ": layer size (" << layer_size << ") not valid " << std::endl;
						return false;
					}
					//jump space
					while (conf_skip_line_space(ptr)
						|| conf_skip_line_comment(ptr)
						|| conf_skip_multilines_comment(line, ptr));
					//activation function (default linear)
					std::string layer_af = conf_name(ptr);
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
					while (conf_skip_line_space(ptr)
						|| conf_skip_line_comment(ptr)
						|| conf_skip_multilines_comment(line, ptr));
					//activation function (default linear)
					std::string layer_af = conf_name(ptr);
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

			}
	};
	//////////////////////////////////////////////////////////////////////////////////
	bool Parameters::from_config(const std::string& source)
	{
		//ptr
		const char* ptr = source.c_str();
		size_t line = 0;
		//jump
		conf_skip_space_and_comments(line, ptr);
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
			if (command == "args")
			{
				do
				{
					if (!ParametersParseHelp::conf_parse_arg(m_params_info, line, ptr)) return false;
				}
				while (*ptr && (*ptr) != '}');
			}
			else if (command == "network")
			{
				do
				{
					if (!ParametersParseHelp::conf_parse_net(*this, line, ptr)) return false;
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