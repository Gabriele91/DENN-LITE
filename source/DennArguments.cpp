#include "DennArguments.h"
#include <cctype>

namespace Denn
{
	/////////////////////////////////////////////////////////////////////////
	MainArguments::MainArguments(int nargs, const char** vargs)
	: m_n_arg(nargs)
	, m_rem_arg(nargs)
	, m_pointer(vargs)
	{
	}

	const char* MainArguments::get_string()
	{
		assert(m_rem_arg);
		--m_rem_arg;
		return *(m_pointer++);
	}

	bool MainArguments::get_bool()
	{
		std::string arg = get_string();
		std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
		return arg == std::string("true")
			|| arg == std::string("yes")
			|| arg == std::string("t")
			|| arg == std::string("y");
	}

	int MainArguments::get_int()
	{
		return atoi(get_string());
	}

	double MainArguments::get_double()
	{
		return atof(get_string());
	}

	bool MainArguments::eof() const
	{
		return m_rem_arg == 0;
	}

	bool MainArguments::back()
	{
		if (m_n_arg <= m_rem_arg) return false;
		++m_rem_arg;
		--m_pointer;
		return true;
	}

	bool MainArguments::end_vals() const
	{
		return eof() || ((*m_pointer)[0] == '-' && !std::isdigit((*m_pointer)[1]));
	}
	/////////////////////////////////////////////////////////////////////////
	StringArguments::StringArguments(const char* values, const std::vector<char>& end_vals)
	: m_end_vals(end_vals)
	, m_values(values)
	, m_values_start(values)
	{
	}

	const char* StringArguments::get_string()
	{
		assert(eof());
		const char* value = m_values;
		next_arg();
		return value;
	}

	bool StringArguments::get_bool()
	{
		std::string arg = get_string();
		std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
		return arg == std::string("true")
			|| arg == std::string("yes")
			|| arg == std::string("t")
			|| arg == std::string("y");
	}

	int StringArguments::get_int()
	{
		return atoi(get_string());
	}

	double StringArguments::get_double()
	{
		return atof(get_string());
	}

	bool StringArguments::eof() const
	{
		return (*m_values) != '\0';
	}
	
	bool StringArguments::back()
	{
		if (m_values == m_values_start) return false;
		prev_arg();
		return true;
	}

	bool StringArguments::end_vals() const
	{
		if (eof()) return true;
		for (char end : m_end_vals) 
			if ((*m_values) == end) return true;
		return false;
	}
	
	void StringArguments::next_arg()
	{
		//jump value
		while (!std::isspace(*m_values) && !end_vals())
		{
			++m_values;
		}
		//jump spaces
		while (std::isspace(*m_values) && !end_vals())
		{
			++m_values;
		}
	}
	void StringArguments::prev_arg()
	{
		//jump value
		while (!std::isspace(*m_values) && !end_vals() && m_values != m_values_start)
		{
			--m_values;
		}
		//jump spaces
		while (std::isspace(*m_values) && !end_vals())
		{
			--m_values;
		}

	}
	/////////////////////////////////////////////////////////////////////////
}