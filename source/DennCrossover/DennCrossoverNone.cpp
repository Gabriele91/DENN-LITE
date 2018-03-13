#include "DennCrossover.h"
#include "DennParameters.h"
#include "DennRandom.h"

namespace Denn
{
	class None : public Crossover
	{
	public:
		None(const DennAlgorithm& algorithm) : Crossover(algorithm) {}

		virtual void operator()(const Population& population, size_t id_target, Individual& i_mutant)
		{
			//ref
			const auto& i_target = *population[id_target];
			//..
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			//copy old level
			if( i_layer != current_layer_to_train() )
			{
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					i_mutant[i_layer][m] = i_target[i_layer][m];
				}
				continue;
			}
			//none
		}
	};
	REGISTERED_CROSSOVER(None, "none")
}