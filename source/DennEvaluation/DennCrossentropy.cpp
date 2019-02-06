#include "DennEvaluation.h"
#include "DennParameters.h"
#include "DennCostFunction.h"
#include "DennRandom.h"

namespace Denn
{
	class CrossEntropy : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			Matrix output = individual.m_network.feedforward(dataset.features());
			return CostFunction::cross_entropy_logistic_by_cols(
                  dataset.labels()
                , output
			).mean();
        }
    };
    REGISTERED_EVALUATION(CrossEntropy,"cross_entropy")

	class CrossEntropyLogistic : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			Matrix output = individual.m_network.feedforward(dataset.features());
			return CostFunction::cross_entropy_logistic_by_cols(
					  dataset.labels()
					, output
			).mean();
        }
    };
    REGISTERED_EVALUATION(CrossEntropyLogistic,"cross_entropy_logistic")

    
}