#pragma once
#include <cassert>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>
#include <memory>

namespace Denn
{
	namespace Random
	{
		//Get generator (per thread)
		extern std::mt19937& thread_random_generator();

		//random integer in [0,size) (thread safe)
		extern int irand(int max);

		//random integer in [0,size) (thread safe)
		extern size_t index_rand(size_t max);

		//random value in flooting point [min,max] (thread safe)
		extern double random(double min = 0.0,double max = 1.0);

		//random in range of [0,size], start index = diff+1, diff is excluded (thread safe)
		extern int rand_range_circle_diff_from(int size, int diff);

		//n random indices (they are different from each other and there are not equal to 'diff') (thread safe)
		extern void n_rand_different_indices(int size, int diff, std::vector< int >& indexs, int indexs_size);

		//random deck
		class RandomDeck
		{
			size_t 					     m_size{ 0 };
			size_t					     m_k   { 0 };
			std::unique_ptr < size_t[] > m_deck{ nullptr };

		public:

			RandomDeck(){}

			RandomDeck(size_t size);

			size_t get_random_id(size_t target);

			void reset();

			void resize(size_t size);

		};

	};
}