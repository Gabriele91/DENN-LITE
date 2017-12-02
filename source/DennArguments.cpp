#include "DennArguments.h"


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

	bool Arguments::get_bool()
	{
		std::string arg = get_string();
		std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
		return arg == std::string("true")
			|| arg == std::string("yes")
			|| arg == std::string("t")
			|| arg == std::string("y");
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
}