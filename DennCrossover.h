#pragma once
#include "Config.h"
#include "DennPopulation.h"

namespace Denn
{
    enum class CrossOverType
    {
        CR_BIN,
        CR_EXP		
    };

    template < typename Individual >
	class Crossover
	{
		public: 
		virtual void operator()(Individual& target,Individual& mutant)= 0; 
	};

    template < typename Individual >
	class Bin : public Crossover< Individual >
	{
		public: 
		virtual void operator()(Individual& i_target,Individual& i_mutant)
		{
			//baias
			const auto& cr = i_mutant.m_cr;
			//for each layers
			for (size_t i_layer=0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m!= i_target[i_layer].size(); ++m)
				{
					//elements
					auto w_target = i_target[i_layer][m];
					auto w_mutant  = i_mutant[i_layer][m];
					//random i
					size_t e_rand = RandomIndices::irand(w_target.size());
					//CROSS
					for (size_t e = 0; e != w_target.size(); ++e)
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

    template < typename Individual >
	class Exp : public Crossover< Individual >
	{
		public: 
		virtual void operator()(Individual& i_target,Individual& i_mutant)
		{
			//baias
			const auto& cr = i_mutant.m_cr;
			//for each layers
			for (size_t i_layer=0; i_layer != i_target.size(); ++i_layer)
			{
				//weights and baias
				for (size_t m = 0; m!= i_target[i_layer].size(); ++m)
				{
					//elements
					auto w_target = i_target[i_layer][m];
					auto w_mutant  = i_mutant[i_layer][m];
					//random i
					size_t e_rand = RandomIndices::irand(w_target.size());
					//event
					bool copy_event = false;
					//CROSS
					for (size_t e = 0; e != w_target.size(); ++e)
					{
						//crossover
						//!(RandomIndices::random() < cr || e_rand == e)
						copy_event |= ( e_rand != e && cr <= RandomIndices::random());
						//copy all vector
						if (copy_event) 
						{
							w_mutant(e) = w_target(e);
						}
					}
				}
			}
		}
	};
}