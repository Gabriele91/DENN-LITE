#pragma once
#include <cassert>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>

namespace Denn
{
	namespace RandomIndices
	{
		//Get generator (per thread)
		extern std::mt19937& thread_random_generator();

		//random integer in [0,size) (thread safe)
		extern int irand(int max);

		//random value in flooting point [0,1] (thread safe)
		extern double random_0_to_1();

		//random in range of [0,size], start index = diff+1, diff is excluded (thread safe)
		extern int rand_range_circle_diff_from(int size, int diff);

		//n random indices (they are different from each other and there are not equal to 'diff') (thread safe)
		extern void n_rand_different_indices(int size, int diff, std::vector< int >& indexs, int indexs_size);

	};
}