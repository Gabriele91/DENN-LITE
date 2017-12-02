#include "DennParameters.h"
#include <cctype>

namespace Denn
{
	//////////////////////////////////////////////////////////////////////////////////
	static inline bool conf_is_xdigit(char c)
	{
		return (c >= '0' && c <= '9') || ((c & ~' ') >= 'A' && (c & ~' ') <= 'F');
	}

	static inline bool json_is_space(char c)
	{
		return c == ' ' || (c >= '\t' && c <= '\r');
	}	
	
	static inline int conf_char_to_int(char c)
	{
		return (c <= '9') ? c - '0' : (c & ~' ') - 'A' + 10;
	}
	//////////////////////////////////////////////////////////////////////////////////
	static bool conf_skip_space(size_t& line, const char*& source)
	{
		bool a_space_is_skipped = false;
		while (json_is_space(*source))
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
		conf_skip_space(line, ptr);
		//action
		bool is_a_valid_arg = false;
		//vals			
		for (auto& action : info)
		{
			//test
			if (!action.m_associated_variable) continue;
			if (!action.m_owener.test(ower)) continue;
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
				StringArguments args(ptr, { '\n', '{', '}' });
				if (!action.m_action(args))
				{
					std::cerr << line << ": not valid arguments for command \'" << command << "\'" << std::endl;
					return false;
				}
				//update
				ptr = args.get_ptr();
				conf_skip_space(line, ptr);
				//sub args?
				if (*ptr == '{')
				{
					//jump {
					++ptr;
					conf_skip_space(line, ptr);
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
					conf_skip_space(line, ptr);
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
		conf_skip_space(line, ptr);
	}

	bool Parameters::from_config(const std::string& source)
	{
		//ptr
		const char* ptr = source.c_str();
		size_t line = 0;
		//jump
		conf_skip_space(line, ptr);
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
			conf_skip_space(line, ptr);
			//test
			if (*ptr != '{')
			{
				std::cerr << line << ": { not found" << std::endl;
				return false;
			}
			//jump {
			++ptr;
			conf_skip_space(line, ptr);
			//type
			if (command == "args")
			{
				do
				{
					if (!conf_parse_arg(m_params_info, line, ptr)) return false;
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
			conf_skip_space(line, ptr);
			//loop
		}
		return true;
	}
}