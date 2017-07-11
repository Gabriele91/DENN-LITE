#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"

namespace Denn
{
	class DEMethod : public EvolutionMethod
	{
	public:

		DEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void update_f_cr
		(
			DoubleBufferPopulation& population
			, int target
			, Individual& i_output
		)
		override
		{
			const Population& parents = population.parents();
			const Individual& i_target = *parents[target];
			//copy
			i_output.m_f = i_target.m_f;
			i_output.m_cr = i_target.m_cr;
		}

		virtual	void selection(DoubleBufferPopulation& population)
		{
			population.the_best_sons_become_parents();
		}

	};
	REGISTERED_EVOLUTION_METHOD(DEMethod,"DE")

	class JDEMethod : public EvolutionMethod
	{
	public:

		JDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void update_f_cr
		(     
			  DoubleBufferPopulation& population
			, int target
			, Individual& i_output
		) 
		override
		{
			//vectors
			const Population& parents = population.parents();
			const Individual& i_target = *parents[target];
			const Parameters& parameters = m_algorithm.parameters();
			//f JDE
			if (Random::uniform() < Scalar(parameters.m_jde_f))
				i_output.m_f = Scalar(Random::uniform(0.0, 2.0));
			else
				i_output.m_f = i_target.m_f;
			//cr JDE
			if (Random::uniform() < Scalar(parameters.m_jde_cr))
				i_output.m_cr = Scalar(Random::uniform());
			else
				i_output.m_cr = i_target.m_cr;
		}

		virtual	void selection(DoubleBufferPopulation& population)
		{
			population.the_best_sons_become_parents();
		}

	};
	REGISTERED_EVOLUTION_METHOD(JDEMethod, "JDE")


	class JADEMethod : public EvolutionMethod
	{
	public:

		JADEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) 
		{
			m_archive_max_size = m_algorithm.parameters().m_archive_size;
			m_c_adapt          = m_algorithm.parameters().m_f_cr_adapt;
			m_mutation_f       = Scalar(0.5);
			m_mutation_cr      = Scalar(0.5);
		}
		
		virtual void reset() override
		{
			m_archive.clear();
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//sort parents
			dpopulation.parents().sort();
		}

		virtual void update_f_cr
		(
			  DoubleBufferPopulation& dpopulation
			, int target
			, Individual& i_output
		)
		override
		{
			//Compute F
			//JADE REF:  Cauchy  distribution  with  location  parameter μF and scale parameter 0.1
			//           Fi=randci(μF,0.1) and  then  truncated  to  be  1  if Fi≥1  or  regenerated  if Fi ≤ 0
			Scalar v;
			do v = Random::cauchy(m_mutation_f, 0.1); while (v <= 0);
			i_output.m_f = Denn::sature(v);
			//Cr
			i_output.m_cr = Denn::sature(Random::normal(m_mutation_cr, 0.1));
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation)
		{
			Scalar sum_f  = 0;
			Scalar sum_f2 = 0;
			Scalar sum_cr = 0;
			size_t n_discarded = 0;

			for (size_t i=0; i!= dpopulation.sons().size(); ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son    = dpopulation.sons()[i];
				if (father->m_eval >= son->m_eval)
				{
					if (m_archive_max_size)
					{
						if (m_archive.size() < m_archive_max_size)
						{
							m_archive.push_back(father->copy());
						}
						else if (Random::uniform() < 0.5)
						{
							(*m_archive[Random::irand(m_archive.size())]) = (*father);
						}
					}
					sum_f  += son->m_f;
					sum_f2 += son->m_f * son->m_f;
					sum_cr += son->m_cr;
					++n_discarded;
					dpopulation.swap(i);
				}
			}
			//
			m_mutation_f  = Denn::lerp(m_mutation_cr, sum_cr / n_discarded, m_c_adapt);
			m_mutation_cr = Denn::lerp(m_mutation_f,  sum_f2 / sum_f, m_c_adapt);
		}

		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:		
		size_t     m_archive_max_size{ false };
		Scalar     m_c_adapt         { 1.0 };
		Scalar     m_mutation_f      { 0   };
		Scalar     m_mutation_cr     { 0   };
		Population m_archive;

	};
	REGISTERED_EVOLUTION_METHOD(JADEMethod, "JADE")

}