#include "DennMutation.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>
#include <assert.h>
namespace Denn
{
	class NoneMutation : public Mutation
	{
	public:

		NoneMutation(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, size_t id_target, Individual& i_final) override
		{
			//target
			i_final = *population[id_target];
		}
	};
	REGISTERED_MUTATION(NoneMutation, "none")
}