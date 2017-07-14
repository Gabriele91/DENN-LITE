#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"

namespace Denn
{
	class DEMethod : public EvolutionMethod
	{
	public:

		DEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void start() override
		{
			//create mutation/crossover
			m_mutation = MutationFactory::create(m_algorithm.parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
		}

		virtual void create_a_individual
		(
			DoubleBufferPopulation& population
			, int target
			, Individual& i_output
		)
		override
		{
			const Population& parents  = population.parents();
			const Individual& i_target = *parents[target];
			//copy
			i_output.m_f  = i_target.m_f;
			i_output.m_cr = i_target.m_cr;
			//call muation
			(*m_mutation) (parents, target, i_output);
			//call crossover
			(*m_crossover)(parents, target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			population.the_best_sons_become_parents();
		}

	private:

		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(DEMethod,"DE")

	class JDEMethod : public EvolutionMethod
	{
	public:

		JDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}
	
		virtual void start() override
		{
			//create mutation/crossover
			m_mutation = MutationFactory::create(m_algorithm.parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
		}

		virtual void create_a_individual
		(     
			  DoubleBufferPopulation& dpopulation
			, int target
			, Individual& i_output
		) 
		override
		{
			//vectors
			const Population& parents    = dpopulation.parents();
			const Individual& i_target   = *parents[target];
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
			//call muation
			(*m_mutation) (parents, target, i_output);
			//call crossover
			(*m_crossover)(parents, target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			population.the_best_sons_become_parents();
		}

	protected:

		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

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
		
		virtual void start() override
		{
			//reinit
			m_mutation_f = Scalar(0.5);
			m_mutation_cr = Scalar(0.5);
			//clear
			m_archive.clear();
			//create mutation/crossover
			m_mutation = MutationFactory::create(m_algorithm.parameters().m_mutation_type, m_algorithm);
			m_crossover = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
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

		virtual void create_a_individual
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
			//call muation
			(*m_mutation) (dpopulation.parents(), target, i_output);
			//call crossover
			(*m_crossover)(dpopulation.parents(), target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
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
			m_mutation_cr  = Denn::lerp(m_mutation_cr, sum_cr / n_discarded, m_c_adapt);
			m_mutation_f   = Denn::lerp(m_mutation_f,  sum_f2 / sum_f, m_c_adapt);
		}

		virtual const VariantRef get_context_data() const override
		{
			return VariantRef(m_archive);
		}

	protected:		

		size_t          m_archive_max_size{ false };
		Scalar          m_c_adapt         { Scalar(1.0) };
		Scalar          m_mutation_f      { Scalar(0.5) };
		Scalar          m_mutation_cr     { Scalar(0.5) };
		Population	    m_archive;
		Mutation::SPtr  m_mutation;
		Crossover::SPtr m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(JADEMethod, "JADE")

	class SaDEMethod : public EvolutionMethod
	{
	public:

		SaDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			//m_epoct = m_algorithm.parameters().m_epoct;
		}

		virtual void start() override
		{
			//reinit
			for (size_t i = 0; i != 2; ++i)
			{
				m_nfalse[i] = 0;
				m_ntesut[i] = 0;
				m_p[i]	    = Scalar(0.5);
			}
			//restart counter
			m_curr_epoct = m_epoct;
			//create mutation/crossover
			m_mutation[0] = MutationFactory::create("rand/1", m_algorithm);
			m_mutation[1] = MutationFactory::create("current_to_best/1", m_algorithm);
			m_crossover   = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
		}

		virtual void start_a_gen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//Update F/CR??
		}

		virtual void start_a_subgen_pass(DoubleBufferPopulation& dpopulation) override
		{
			//none
		}

		virtual void create_a_individual
		(
			DoubleBufferPopulation& dpopulation
			, int target
			, Individual& i_output
		)
		override
		{
			//todo
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			//todo
			for (size_t i = 0; i != dpopulation.sons().size(); ++i)
			{
				Individual::SPtr father = dpopulation.parents()[i];
				Individual::SPtr son = dpopulation.sons()[i];
				if (father->m_eval >= son->m_eval)
				{
					//todo
					dpopulation.swap(i);
				}
			}
			//todo
		}

		virtual const VariantRef get_context_data() const override
		{
			//todo
			return VariantRef();
		}

	protected:

		size_t			   m_epoct     { 50   };
		size_t			   m_curr_epoct{ 50   };
		size_t             m_nfalse[2] { 0, 0 };
		size_t             m_ntesut[2] { 0, 0 };
		Scalar			   m_p[2]      { Scalar(0.5), Scalar(0.5) };
		Population	       m_archive;
		Mutation::SPtr     m_mutation[2];
		Crossover::SPtr    m_crossover;

	};
	REGISTERED_EVOLUTION_METHOD(SaDEMethod, "SADE")

}