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

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//target
			i_final = *population[id_target];
		}
	};
	REGISTERED_MUTATION(NoneMutation, "none")

	class RandOne : public Mutation
	{
	public:

		RandOne(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//init generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
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
					w_final = ((x_a - x_b) * f + x_c).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}
	};
	REGISTERED_MUTATION(RandOne, "rand/1")

	class RandTwo : public Mutation
	{
	public:

		RandTwo(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//get generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
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
					w_final = (((x_a - x_b) + (x_c - x_d)) * f + x_e).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}
	};
	REGISTERED_MUTATION(RandTwo, "rand/2")

	class BestOne : public Mutation
	{
	public:

		BestOne(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];			
			//best
			size_t id_best;
			Scalar eval_best;
			population.best(id_best,eval_best);
			const Individual& i_best = *population[id_best];
			//get generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_best)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_best)];
					//
					Matrix& w_final      = i_final[i_layer][m];
					const Matrix& x_best = i_best[i_layer][m];
					const Matrix& x_a    = nn_a[i_layer][m];
					const Matrix& x_b    = nn_b[i_layer][m];
					w_final = ((x_a - x_b) * f + x_best).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}
	};
	REGISTERED_MUTATION(BestOne, "best/1")

	class BestTwo : public Mutation
	{
	public:

		BestTwo(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best
			size_t id_best;
			Scalar eval_best;
			population.best(id_best,eval_best);
			const Individual& i_best = *population[id_best];
			//get generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_best)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_best)];
					const Individual& nn_c = *population[rand_deck.get_random_id(id_best)];
					const Individual& nn_d = *population[rand_deck.get_random_id(id_best)];
					//
					Matrix& w_final      = i_final[i_layer][m];
					const Matrix& w_best = i_best[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					const Matrix& x_c = nn_c[i_layer][m];
					const Matrix& x_d = nn_d[i_layer][m];
					w_final = (((x_a - x_b) + (x_c - x_d)) * f + w_best).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}
	};
	REGISTERED_MUTATION(BestTwo, "best/2")
	
	class CurrentToBest : public Mutation
	{
	public:

		CurrentToBest(const DennAlgorithm& algorithm):Mutation(algorithm) 
		{ 
		}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best (n.b. JADE sort population from best to worst)
			const Individual& i_best = *population.best();
			//get generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
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
					//from_archive ? archive[r2] : father(r2);
					const Matrix& w_target = i_target[i_layer][m];
						  Matrix& w_final  = i_final[i_layer][m];
					const Matrix& w_best   = i_best[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					w_final = ( w_target + ((w_best - w_target) + (x_a - x_b)) * f ).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}

	};
	REGISTERED_MUTATION(CurrentToBest, "current_to_best/1")

	class CurrentToPBest : public Mutation
	{
	public:

		//required sort
		virtual bool required_sort() const override { return true; }

		CurrentToPBest(const DennAlgorithm& algorithm):Mutation(algorithm) 
		{ 
			//JADE REQUIRED
			assert(*m_algorithm.parameters().m_evolution_type == "JADE");
			//Get JADE archive
			m_archive = m_algorithm.evolution_method().get_context_data().get_ptr<Population>();
			//Get percentage of best individuals
			m_perc_of_best = m_algorithm.parameters().m_perc_of_best;
		}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//alias
			const auto& f = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best (n.b. JADE sort population from best to worst)
			const Individual& i_best = *population[random(id_target).irand(size_t(m_perc_of_best*(Scalar)population.size()))];
			//get generator
			auto& rand_deck = random(id_target).deck();
			//set population size in deck
			rand_deck.reinit(population.size());
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//archive
					size_t rand_b = random(id_target).irand(m_archive->size() + population.size() - 2);
					bool get_from_archive = rand_b < m_archive->size();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = get_from_archive ? *(*m_archive)[rand_b] : *population[rand_deck.get_random_id(id_target)];
					//from_archive ? archive[r2] : father(r2);
					const Matrix& w_target = i_target[i_layer][m];
						  Matrix& w_final  = i_final[i_layer][m];
					const Matrix& w_best   = i_best[i_layer][m];
					const Matrix& x_a = nn_a[i_layer][m];
					const Matrix& x_b = nn_b[i_layer][m];
					w_final = ( w_target + ((w_best - w_target) + (x_a - x_b)) * f ).unaryExpr(m_algorithm.clamp_function());
				}
			}
		}

	protected:

		Scalar m_perc_of_best;
		const Population* m_archive;

	};
	REGISTERED_MUTATION(CurrentToPBest, "curr_p_best")

	class DEGL : public Mutation
	{
	public:

		DEGL(const DennAlgorithm& algorithm) :Mutation(algorithm) {}

		virtual void operator()(const Population& population, int id_target, Individual& i_final)
		{
			//... page 6 
			//https://pdfs.semanticscholar.org/5523/8adbd3d78dc83cf906240727be02f6560470.pdf
			//alias
			const auto& f = i_final.m_f;
			Scalar scalar_weight= *m_algorithm.parameters().m_degl_scalar_weight;
			size_t neighborhood = *m_algorithm.parameters().m_degl_neighborhood;
			//target
			const Individual& i_target = *population[id_target];
			//best
			//best
			size_t id_g_best;
			Scalar eval_g_best;
			population.best(id_g_best,eval_g_best);
			const Individual& g_best = *population[id_g_best];
			//local best
			long nn                     =  (long)neighborhood;
			long np                     =  (long)population.size();
			long id_l_best				=  id_target;
			for(long k=-nn; k!=(nn+1); ++k)
			{
				long i = Denn::positive_mod(k + id_target, np);
				if( population[i]->m_eval <  population[id_l_best]->m_eval) id_l_best = i;
			}
			//local best ref
			const Individual& l_best = *population[id_l_best];	
			//get generator
			auto& rand_deck				 = random(id_target).deck();
			auto& rand_deck_ring_segment = random(id_target).deck_ring_segment();
			//set population size in deck
			rand_deck.reinit(population.size());
			rand_deck_ring_segment.reinit(population.size(), id_target, neighborhood);
			//for each layers
			for (size_t i_layer = 0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m != i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					rand_deck_ring_segment.reset();
					//do cross + mutation
					const Individual& nn_g_a = *population[rand_deck.get_random_id(id_target)]; //a != target
					const Individual& nn_g_b = *population[rand_deck.get_random_id(id_target)]; //b != target 

					const Individual& nn_l_a = *population[rand_deck_ring_segment.get_random_id()];//local a != target
					const Individual& nn_l_b = *population[rand_deck_ring_segment.get_random_id()];//local b != target
					//									
					const Matrix& w_target = i_target[i_layer][m];
					const Matrix& w_g_best = g_best[i_layer][m];
					const Matrix& w_l_best = l_best[i_layer][m];
					      Matrix& w_final  = i_final[i_layer][m];

					const Matrix& x_g_a = nn_g_a[i_layer][m];
					const Matrix& x_g_b = nn_g_b[i_layer][m];					
					const Matrix& x_l_a = nn_l_a[i_layer][m];
					const Matrix& x_l_b = nn_l_b[i_layer][m];

					//global
					Matrix g_m = ( w_target + ((w_g_best - w_target) + (x_g_a - x_g_b)) * f );

					//local
					Matrix l_m = ( w_target + ((w_l_best - w_target) + (x_l_a - x_l_b)) * f );

					//final (lerp)
					//from the DEGL's peper
					//lambda = 1 -> g_m (aka rand-to-best/1)
					//lambda = 0 -> l_m
					Scalar* w_final_array = w_final.data();
					Scalar* w_g_m_array   = g_m.data();
					Scalar* w_l_m_array   = l_m.data();
					for(Matrix::Index i=0;i!=w_final.size();++i)
						w_final_array[i] = Denn::lerp(w_l_m_array[i], w_g_m_array[i], scalar_weight);

					//clamp
					w_final.unaryExpr(m_algorithm.clamp_function());
				}
			}
		}
	};
	REGISTERED_MUTATION(DEGL, "degl")
}