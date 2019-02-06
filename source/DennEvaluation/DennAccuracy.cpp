#include "DennEvaluation.h"
#include "DennParameters.h"
#include "DennCostFunction.h"
#include "DennRandom.h"

namespace Denn
{
	class Accuracy : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return false; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			const Matrix& x = individual.m_network.feedforward(dataset.features());
            return CostFunction::accuracy_cols(x,dataset.labels());
        }
		
    };
    REGISTERED_EVALUATION(Accuracy,"accuracy")
    
	class InverseAccuracy : public Evaluation
	{
	public:
        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& dataset)
        {
			const Matrix& x = individual.m_network.feedforward(dataset.features());
            return CostFunction::inverse_accuracy_cols(x,dataset.labels());
        }
		
    };
	REGISTERED_EVALUATION(InverseAccuracy,"inverse_accuracy")
}