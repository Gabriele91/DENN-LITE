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
			return CostFunction::cross_entropy(
                  dataset.labels()
                , individual.m_network.apply(dataset.features())
			);
        }
    };
    REGISTERED_EVALUATION(CrossEntropy,"cross_entropy")

	class SoftmaxCrossEntropy : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			return CostFunction::cross_entropy(
                  dataset.labels()
                , CostFunction::softmax_row_samples<Matrix>(individual.m_network.apply(dataset.features()))
			);
        }
    };
    REGISTERED_EVALUATION(SoftmaxCrossEntropy,"softmax_cross_entropy")

	class CrossEntropyLogistic : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			return CostFunction::cross_entropy_logistic_regression(
					  dataset.labels()
					, individual.m_network.apply(dataset.features())
			);
        }
    };
    REGISTERED_EVALUATION(CrossEntropyLogistic,"cross_entropy_logistic")

    class SoftmaxCrossEntropyLogistic : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			return CostFunction::cross_entropy_logistic_regression(
					  dataset.labels()
					, CostFunction::softmax_row_samples<Matrix>(individual.m_network.apply(dataset.features()))
			);
        }
    };
    REGISTERED_EVALUATION(SoftmaxCrossEntropyLogistic,"softmax_cross_entropy_logistic")
    
}