#include "DennCrossover.h"
#include "DennParameters.h"
#include "DennRandom.h"

namespace Denn
{
	class Interm : public Crossover
	{
	public:
		Interm(const DennAlgorithm& algorithm) : Crossover(algorithm) {}

		virtual void operator()(const Population& population, size_t id_target, Individual& i_mutant)
		{
			//baias
			const auto& i_target = *population[id_target];
			// const auto& cr = i_mutant.m_cr;
			// for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//elements
					auto w_target = i_target[i_layer][m].array();
					auto w_mutant = i_mutant[i_layer][m].array();
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						Scalar factor = random(id_target).uniform();
						w_mutant(e) = w_target(e) + factor * (w_mutant(e) - w_target(e));
					}
				}
			}
		}
	};
	REGISTERED_CROSSOVER(Interm, "interm")

	class BinInterm : public Crossover
	{
	public:
		BinInterm(const DennAlgorithm& algorithm) : Crossover(algorithm) {}

		virtual void operator()(const Population& population, size_t id_target, Individual& i_mutant)
		{
			//baias
			const auto& i_target = *population[id_target];
			const auto& cr = i_mutant.m_cr;
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//elements
					auto w_target = i_target[i_layer][m].array();
					auto w_mutant = i_mutant[i_layer][m].array();
					//random i
					size_t e_rand = random(id_target).index_rand(w_target.size());
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						//crossover
						//!(RandomIndices::random() < cr || e_rand == e)
						if (e_rand != e && cr <= random(id_target).uniform())
						{
							Scalar factor = Scalar(random(id_target).uniform());
							w_mutant(e) = w_target(e) + factor * (w_mutant(e) - w_target(e));
						}
					}
				}
			}
		}
	};
	REGISTERED_CROSSOVER(BinInterm,"bin_interm")

}
