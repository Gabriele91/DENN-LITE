#include "DennParameters.h"
#include "DennIndividual.h"
#include "DennPopulation.h"
#include "DennAlgorithm.h"
#include "DennEvolutionMethod.h"
#include "DennMutation.h"
#include "DennCrossover.h"
namespace Denn
{
	class NoneMethod : public EvolutionMethod
	{
	public:

		NoneMethod(const DennAlgorithm& algorithm) : EvolutionMethod(algorithm) {}

		virtual void create_a_individual
		(
			  DoubleBufferPopulation& population
			, size_t i_target
			, Individual& i_output
		)
		override
		{
			//none
		}

		virtual	void selection(DoubleBufferPopulation& population) override
		{
			//none
		}

	private:

	};
	REGISTERED_EVOLUTION_METHOD(NoneMethod,"NONE")
}