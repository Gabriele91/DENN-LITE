#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include "DennCostFunction.h"
#include "DennDump.h"
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
			//update
			i_output.m_network.fit(
				m_algorithm.current_batch().features(),
				m_algorithm.current_batch().labels(),
				SGD(m_learning_rate, m_regularize)
			);
			//std::cout << Dump::json_matrix(m_algorithm.current_batch().features().leftCols(1)) << std::endl;
			//std::cout << Dump::json_matrix(m_algorithm.current_batch().labels().leftCols(1)) << std::endl;
			//std::cout << Dump::json_matrix(i_output.m_network[i_output.m_network.size()-1].ff_output().leftCols(1)) << std::endl;
			//fake swap 
			dpopulation.swap(i_target);
		}

		virtual	void selection(DoubleBufferPopulation& dpopulation) override
		{
			dpopulation.swap_all();
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