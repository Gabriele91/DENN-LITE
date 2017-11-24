#include "DennEvaluation.h"
#include "DennParameters.h"
#include "DennCostFunction.h"
#include "DennRandom.h"

namespace Denn
{
	class Accuary : public Evaluation
	{
	public:
		Accuary(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return false; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
            auto x = individual.m_network.apply(dataset.features());
            return CostFunction::accuracy(x,dataset.labels());
        }
		
    };
    REGISTERED_EVALUATION(Accuary,"accuary")
    
	class InverseAccuracy : public Evaluation
	{
	public:
		InverseAccuracy(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
            auto x = individual.m_network.apply(dataset.features());
            return CostFunction::inverse_accuracy(x,dataset.labels());
        }
		
    };
	REGISTERED_EVALUATION(InverseAccuracy,"inverse_accuracy")
}