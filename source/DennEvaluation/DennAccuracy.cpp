#include "DennEvaluation.h"
#include "DennParameters.h"
#include "DennCostFunction.h"
#include "DennRandom.h"

namespace Denn
{
	class Accuary : public Evaluation
	{
	public:
		Bin(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return false; }
        virtual Scalar operator () (const Individual& individual, const DataSet& set)
        {
            auto x = individual->m_network.apply(ds.features());
            return CostFunction::accuracy(x,set.labels());
        }
		
    };
    REGISTERED_EVALUATION(Accuary,"accuary")
    
	class InvAccuary : public Evaluation
	{
	public:
		Bin(const DennAlgorithm& algorithm) : Evaluation(algorithm) {}

        //methods
        virtual bool minimize() const { return true; }
        virtual Scalar operator () (const Individual& individual, const DataSet& set)
        {
            auto x = individual->m_network.apply(ds.features());
            return CostFunction::inv_accuary(x,set.labels());
        }
		
    };
	REGISTERED_EVALUATION(InvAccuary,"inv_accuary")
}