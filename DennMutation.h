#pragma once
#include "Config.h"
#include "DennParameters.h"
#include "DennPopulation.h"

namespace Denn
{

    enum class MutationType
    {
        MT_RAND_ONE,
        MT_BEST_ONE		
    };	

    template <
        typename Parameters, 
        typename Population, 
        typename Individual 
    >
    class Mutation 
	{ 
        //alias
        using ScalarType = typename Individual::ScalarType;

		public: 

		Mutation(const Parameters& parameters) : m_parameters(parameters){}
		virtual void operator()(const Population& population,int id_target,Individual& output)= 0; 

		protected:
		//utils
		ScalarType f_clamp(ScalarType value) const
		{
			ScalarType cmin = m_parameters.m_clamp_min;
			ScalarType cmax = m_parameters.m_clamp_max;
			return Denn::clamp<ScalarType>(value, cmin, cmax);
		}
		//attributes
		const Parameters& m_parameters;
	};

    template <
        typename Parameters, 
        typename Population, 
        typename Individual 
    >
    class RandOne : public Mutation< Parameters, Population, Individual >
	{ 
        //alias
        using ScalarType = typename Individual::ScalarType;

		public: 

		RandOne(const Parameters& parameters):Mutation< Parameters, Population, Individual >(parameters){}

		virtual void operator()(const Population& population,int id_target,Individual& i_final)
		{
			//alias
			const auto& f  = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer=0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m!= i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_c = *population[rand_deck.get_random_id(id_target)];
					//
					auto w_final  = i_final[i_layer][m];
					auto w_lr_a   = nn_a[i_layer][m];
					auto w_lr_b   = nn_b[i_layer][m];
					auto w_lr_c   = nn_c[i_layer][m];
					//function
					for (size_t e = 0; e != w_lr_a.size(); ++e) 
						w_final(e) = this->f_clamp((w_lr_a(e) - w_lr_b(e)) * f + w_lr_c(e));
				}
			}
		}
	};


    template <
        typename Parameters, 
        typename Population, 
        typename Individual 
    >
	class BestOne : public Mutation< Parameters, Population, Individual >
	{ 
        //alias
        using ScalarType = typename Individual::ScalarType;

		public: 

		BestOne(const Parameters& parameters):Mutation< Parameters, Population, Individual >(parameters){}

		virtual void operator()(const Population& population,int id_target,Individual& i_final)
		{
			//alias
			const auto& f  = i_final.m_f;
			//target
			const Individual& i_target = *population[id_target];
			//best
			const Individual& i_best = *population.best();
			//init generator
			static thread_local RandomIndices::RandomDeck rand_deck;
			//set population size in deck
			rand_deck.resize(population.size());
			//for each layers
			for (size_t i_layer=0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m!= i_target[i_layer].size(); ++m)
				{
					//do rand
					rand_deck.reset();
					//do cross + mutation
					const Individual& nn_a = *population[rand_deck.get_random_id(id_target)];
					const Individual& nn_b = *population[rand_deck.get_random_id(id_target)];
					//
					auto w_final  = i_final[i_layer][m];
					auto w_lr_best= i_best[i_layer][m];
					auto w_lr_a   = nn_a[i_layer][m];
					auto w_lr_b   = nn_b[i_layer][m];
					//function
					for (size_t e = 0; e != w_lr_a.size(); ++e) 
						w_final(e) = this->f_clamp((w_lr_a(e) - w_lr_b(e)) * f + w_lr_best(e));
				}
			}
		}
	};
}