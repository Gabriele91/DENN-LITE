#include "DennParameters.h"

namespace Denn
{
	bool Parameters::from_args(int nargs, const char **vargs)
	{
		MainArguments args(nargs, vargs);
		//start
		while (!args.eof())
		{
			bool is_a_valid_arg = false;
			bool parameters_arguments_are_correct = false;
			const char *p = args.get_string();
			for (auto& action : m_params_info)
			{
				if (compare_n_args(action.m_arg_key, p))
				{
					parameters_arguments_are_correct = action.m_action(args);
					is_a_valid_arg = true;
					break;
				}
			}
			if (!is_a_valid_arg)
			{
				std::cerr << "parameter " << p << " not found" << std::endl;
				return false;
			}
			else if (!parameters_arguments_are_correct)
			{
				std::cerr << "arguments of parameter " << p << " are not correct" << std::endl;
				return false;
			}
		}
		return true;
	}
}