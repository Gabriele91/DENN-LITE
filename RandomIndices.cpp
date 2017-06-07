#include "RandomIndices.h"

namespace Denn
{
namespace RandomIndices
{
	//Get generator (per thread)
	std::mt19937& thread_random_generator()
	{
		static thread_local std::mt19937 generator;
		return generator;
	}
	//random integer in [0,size) (thread safe)
	int irand(int max)
	{
		std::uniform_int_distribution<int> distribution(0, max-1);
		return distribution(thread_random_generator());
	}

	//random value in flooting point [min,max] (thread safe)
	double random(double min,double max)
	{
		std::uniform_real_distribution<double> distribution(min,max);
		return distribution(thread_random_generator());
	}

	//random in range of [0,size], start index = diff+1, diff is excluded (thread safe)
	int rand_range_circle_diff_from(int size, int diff)
	{
		int output = (diff + 1 + irand(size)) % size;
		if (output == diff) return (output + 1) % size;
		return output;
	}

	//n random indices (they are different from each other and there are not equal to 'diff') (thread safe)
	void n_rand_different_indices(int size, int diff, std::vector< int >& indexs, int indexs_size)
	{
		//test
		assert(size >= indexs_size);
		//size of a batch
		int batch_size = size / indexs_size;
		int batch_current = 0;
		int batch_next = 0;
		//compute rands
		for (int i = 0; i != indexs_size; ++i)
		{
			batch_current = batch_size*i;
			batch_next = batch_size*(i + 1);

			if (i == indexs_size - 1)
			{
				int reminder = size % indexs_size;
				batch_next += reminder;
				batch_size += reminder;
			}

			if (batch_current <= diff && diff < batch_next)
			{
				indexs[i] = batch_current + rand_range_circle_diff_from(batch_size, diff - batch_current);
			}
			else
			{
				indexs[i] = batch_current + irand(batch_next - batch_current);
			}
		}
		//shuffle
		std::shuffle(indexs.begin(), indexs.begin() + indexs_size, thread_random_generator());
	}
}
}