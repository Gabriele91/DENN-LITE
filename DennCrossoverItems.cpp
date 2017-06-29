#include "DennCrossover.h"
#include "DennParameters.h"
#include "RandomIndices.h"

namespace Denn
{
	class None : public Crossover
	{
	public:
		None(const Parameters& parameters) : Crossover(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_mutant)
		{
			//no op
		}
	};
	REGISTERED_CROSSOVER(None, "none")

	class Bin : public Crossover
	{
	public:
		Bin(const Parameters& parameters) : Crossover(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_mutant)
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
					size_t e_rand = RandomIndices::index_rand(w_target.size());
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						//crossover
						//!(RandomIndices::random() < cr || e_rand == e)
						if (e_rand != e && cr <= RandomIndices::random())
						{
							w_mutant(e) = w_target(e);
						}
					}
				}
			}
		}
	};
	REGISTERED_CROSSOVER(Bin,"bin")

	class Exp : public Crossover
	{
	public:
		Exp(const Parameters& parameters) : Crossover(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_mutant)
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
					size_t e_rand = RandomIndices::index_rand(w_target.size());
					size_t e_start = RandomIndices::index_rand(w_target.size());
					//event
					bool copy_event = false;
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						//id circ
						size_t e_circ = (e_start + e) % w_target.size();
						//crossover
						//!(RandomIndices::random() < cr || e_rand == e)
						copy_event |= (e_rand != e_circ && cr <= RandomIndices::random());
						//copy all vector
						if (copy_event)
						{
							w_mutant(e_circ) = w_target(e_circ);
						}
					}
				}
			}
		}
	};
	REGISTERED_CROSSOVER(Exp, "exp")

	class Interm : public Crossover
	{
	public:
		Interm(const Parameters& parameters) : Crossover(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_mutant)
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
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						Scalar factor = RandomIndices::random();
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
		BinInterm(const Parameters& parameters) : Crossover(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_mutant)
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
					size_t e_rand = RandomIndices::index_rand(w_target.size());
					//CROSS
					for (decltype(w_target.size()) e = 0; e != w_target.size(); ++e)
					{
						//crossover
						//!(RandomIndices::random() < cr || e_rand == e)
						if (e_rand != e && cr <= RandomIndices::random())
						{
							Scalar factor = RandomIndices::random();
							w_mutant(e) = w_target(e) + factor * (w_mutant(e) - w_target(e));
						}
					}
				}
			}
		}
	};
	REGISTERED_CROSSOVER(BinInterm,"bin_interm")

}