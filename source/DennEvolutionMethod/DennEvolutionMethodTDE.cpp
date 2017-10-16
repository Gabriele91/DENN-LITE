#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
namespace Denn
{
    class TDEMethod : public EvolutionMethod
    {
    public:

        TDEMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

        virtual void start() override
        {
			//create mutation/crossover
            m_mutation = MutationFactory::create(m_algorithm.parameters().m_mutation_type, m_algorithm);
            m_trig_mutation = MutationFactory::create("trig",m_algorithm);
			m_crossover = CrossoverFactory::create(m_algorithm.parameters().m_crossover_type, m_algorithm);
        }

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& population
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			const Population& parents= population.parents();
			const Individual& target = *parents[i_target];
			//copy
			i_output.m_f  = target.m_f;
			i_output.m_cr = target.m_cr;
            //call mutation
            if(random(i_target).uniform() < Scalar(m_algorithm.parameters().m_trig_m))
                (*m_mutation) (parents, i_target, i_output);
            else
                (*m_trig_mutation) (parents, i_target, i_output);
			//call crossover
			(*m_crossover)(parents, i_target, i_output);
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			population.the_best_sons_become_parents();
		}
        
    private:

        Mutation::SPtr m_mutation;
        Mutation::SPtr m_trig_mutation;
        Crossover::SPtr m_crossover;

    };
	REGISTERED_EVOLUTION_METHOD(TDEMethod,"TDE")
}