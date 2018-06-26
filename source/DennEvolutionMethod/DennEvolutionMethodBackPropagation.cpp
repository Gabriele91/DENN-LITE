#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennCostFunction.h"
namespace Denn
{
	class BACKPROPAGATIONMethod : public EvolutionMethod
	{
	public:

		BACKPROPAGATIONMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm)
		{
			m_learning_rate = parameters().m_learning_rate;
			m_regularize    = parameters().m_regularize;
		}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& dpopulation
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//apply on parent
			dpopulation.parents()[i_target]->m_network.backpropagation_gradient_descent
			(
				  m_algorithm.current_batch().features()
				, m_algorithm.current_batch().labels()
				, m_learning_rate
				, m_regularize
			);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			//none
		}

		virtual bool can_reset() override 
		{ 
			return false;
		}

		virtual bool best_from_validation() override 
		{ 
			return false; 
		}
		
	protected:

		Scalar m_learning_rate;
		Scalar m_regularize;
		
	};
	REGISTERED_EVOLUTION_METHOD(BACKPROPAGATIONMethod, "BACKPROPAGATION")
}