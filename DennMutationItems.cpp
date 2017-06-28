#include "DennMutation.h"
#include "DennParameters.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{

	class RandOne : public Mutation
	{
	public:

		RandOne(const Parameters& parameters) :Mutation(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_c = *population[rand_deck.get_random_id(id_target)];
					//
					Matrix& w_final = i_final[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					const Matrix& x_c = nn_c[i_layer][m];
					w_final = ((x_a - x_b) * f + x_c).unaryExpr(m_clamp);
				}
			}
		}
	};
	REGISTERED_MUTATION(RandOne, "rand/1")

	class RandTwo : public Mutation
	{
	public:

		RandTwo(const Parameters& parameters) : Mutation(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_c = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_d = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_e = *population[rand_deck.get_random_id(id_target)];
					//
					Matrix& w_final = i_final[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					const Matrix& x_c = nn_c[i_layer][m];
					const Matrix& x_d = nn_d[i_layer][m];
					const Matrix& x_e = nn_e[i_layer][m];
					w_final = ((x_a - x_b) + (x_c - x_d) * f + x_e).unaryExpr(m_clamp);
				}
			}
		}
	};
	REGISTERED_MUTATION(RandTwo, "rand/2")

	class BestOne : public Mutation
	{
	public:

		BestOne(const Parameters& parameters) : Mutation(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best
			const Individual& i_best = *population.best();
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					//
					Matrix& w_final      = i_final[i_layer][m];
					const Matrix& x_best = i_best[i_layer][m];
					const Matrix& x_a    = nn_a[i_layer][m];
					const Matrix& x_b    = nn_b[i_layer][m];
					w_final = ((x_a - x_b) * f + x_best).unaryExpr(m_clamp);
				}
			}
		}
	};
	REGISTERED_MUTATION(BestOne, "best/1")

	class BestTwo : public Mutation
	{
	public:

		BestTwo(const Parameters& parameters) :Mutation(parameters) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best
			const Individual& i_best = *population.best();
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_c = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_d = *population[rand_deck.get_random_id(id_target)];
					//
					Matrix& w_final      = i_final[i_layer][m];
					const Matrix& w_best = i_best[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					const Matrix& x_c = nn_c[i_layer][m];
					const Matrix& x_d = nn_d[i_layer][m];
					w_final = ((x_a - x_b) + (x_c - x_d) * f + w_best).unaryExpr(m_clamp);
				}
			}
		}
	};
	REGISTERED_MUTATION(BestTwo, "best/2")
}