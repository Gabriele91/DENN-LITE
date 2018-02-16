#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include <array>

namespace Denn
{
	class SaMDEMethod : public EvolutionMethod
	{
	public:

		SaMDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) 
		{
			//get mutations
			for(const std::string& mut_name : parameters().m_mutations_list_type.get()) 
			{
				m_mutations_list.push_back(MutationFactory::create(mut_name, m_algorithm));
			}
			//crossover
			m_crossover = CrossoverFactory::create(parameters().m_crossover_type, m_algorithm);
		}
		
		virtual void start() override
		{
			//init metadata
			init_metadata();
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			for(auto mutation : m_mutations_list)
			{
				if(mutation->required_sort())
				{ 		
					//compare
					auto comp =[&](const Individual::SPtr& li, const Individual::SPtr& ri) -> bool 
							   { return loss_function_compare(li->m_eval,ri->m_eval); };
					//pop
					auto& pop  = dpopulation.parents().as_vector();
					auto& meta = metadata_parents();
					//sort
					auto p = sort_permutation(pop, comp);
					apply_permutation_in_place(pop, p);
					apply_permutation_in_place(meta, p);
					break;
				}
			}
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//new metadata
			size_t method = new_metadata(i_target);
			//set
			set_metadata_to_individual(i_target, method, i_output);
			//call muation
			(*m_mutations_list[method]) (dpopulation.parents(), i_target, i_output);
			//call crossover
			(*m_crossover)(dpopulation.parents(), i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			size_t np = current_np();
			for (size_t i = 0; i != np; ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (loss_function_compare(son->m_eval,father->m_eval))
				{
					dpopulation.swap(i);
					metadata_swap(i);
				}
			}
		}

	protected:		
		//SaDE
		struct Metadata
		{
			Scalar m_f;
			Scalar m_cr;
			Scalar m_v;
		};
		//mdata alias
		using ListScalar		 = std::vector<Scalar>;
		using ListMetadata       = std::vector<Metadata>;
		using ListListMetadata   = std::vector<ListMetadata>;
		using DoubleListMetadata = std::array<ListListMetadata, 2>;
		//population
		DoubleListMetadata  m_double_list_mdata;
		//help metadata
		void init_metadata()
		{
			//for parent & son
			for(ListListMetadata& list_list_mdata : m_double_list_mdata)
			{
				list_list_mdata.clear();
				list_list_mdata.resize(current_np());
				//for all individuals
				for(ListMetadata& list_mdata : list_list_mdata)
				{
					list_mdata.clear();
					list_mdata.resize(m_mutations_list.size());
					//for all methods
					for(Metadata& mdata : list_mdata)
					{
						mdata.m_f = parameters().m_default_f;
						mdata.m_cr = parameters().m_default_f;
						mdata.m_v = Scalar(1.0) / m_mutations_list.size();
					}
				}
			} 
		}
		//swap
		void metadata_swap(size_t i)
		{
			std::swap(m_double_list_mdata[0][i],m_double_list_mdata[1][i]);
		}
		//chouse v id 
		size_t roulette_wheel(ListMetadata& values, size_t i)
		{
			//max
			Scalar max_v = values.begin()->m_v;
			for(auto& v : values) max_v = std::max(max_v,v.m_v);
			//roulette
			for (;;)
			{	
				//id rand		
				size_t id = random(i).uirand(values.size());
				//test
				if (random(i).uniform(0, 1.0) < values[id].m_v / max_v)
					return id;
			}   
		}
		//update metadata
		size_t new_metadata(size_t i)
		{
			//get vector[ f, cr, v ]
			ListListMetadata& l_m_parents = metadata_parents();
			ListListMetadata& l_m_sons = metadata_sons();
			//metadata
			Scalar f_prime = random(i).uniform(0.7, 1.0);
			//get deck
			auto& deck = random(i).deck();
			//reinit
			deck.reinit(current_np());
			deck.reset();
			//rand 
			size_t r1 = deck.get_random_id(i);
			size_t r2 = deck.get_random_id(i);
			size_t r3 = deck.get_random_id(i);
			//Update V[0-k]
			for(size_t k = 0; k!=m_mutations_list.size(); ++k)
			{
				//new
				l_m_sons[i][k].m_v = l_m_parents[r1][k].m_v + f_prime * (l_m_parents[r2][k].m_v - l_m_parents[r3][k].m_v);				
			}
			//chouse winner
			size_t w = roulette_wheel(l_m_sons[i], i);
			//update f and cr
			l_m_sons[i][w].m_f  = l_m_parents[r1][w].m_f + f_prime * (l_m_parents[r2][w].m_f - l_m_parents[r3][w].m_f );
			l_m_sons[i][w].m_cr = l_m_parents[r1][w].m_cr + f_prime * (l_m_parents[r2][w].m_cr - l_m_parents[r3][w].m_cr );
			return w;
		}
		//son/parent
		ListListMetadata& metadata_parents()
		{
			return m_double_list_mdata[0];
		}
		ListListMetadata& metadata_sons()
		{
			return m_double_list_mdata[1];
		}
		ListMetadata& metadata_parent(size_t i)
		{
			return m_double_list_mdata[0][i];
		}
		ListMetadata& metadata_son(size_t i)
		{
			return m_double_list_mdata[1][i];
		}
		//set value to son
		void set_metadata_to_individual(size_t i, size_t w, Individual& ind)
		{
			//meta
			auto& meta = metadata_son(i)[w];
			ind.m_f  = meta.m_f ;
			ind.m_cr = meta.m_cr ;
		}
		//mutation pool
		std::vector<Mutation::SPtr>  m_mutations_list;
		Crossover::SPtr				 m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(SaMDEMethod, "SAMDE")
}