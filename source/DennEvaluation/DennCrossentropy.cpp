#include "DennEvaluation.h"
#include "DennParameters.h"
#include "DennCostFunction.h"
#include "DennRandom.h"

namespace Denn
{
	class CrossEntropy : public Evaluation
	{
	public:
		Bin(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& set)
        {
			return CostFunction::cross_entropy(
                  dataset.labels()
                , individual.m_network.apply(dataset.features())
			);
        }
    };
    REGISTERED_EVALUATION(CrossEntropyLogistic,"cross_entropy")

	class CrossEntropyLogistic : public Evaluation
	{
	public:
		Bin(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& set)
        {
			return CostFunction::cross_entropy_logistic_regression(
					  dataset.labels()
					, individual.m_network.apply(dataset.features())
			);
        }
    };
    REGISTERED_EVALUATION(CrossEntropyLogistic,"cross_entropy_logistic")
    
}