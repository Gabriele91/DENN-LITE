//
// Created by Valerio Belli on 15/11/17.
//

#include "DennGate.h"

namespace Denn
{
    class TwoGate : public Gate
    {
    public:

        TwoGate() : Gate("two", CONST) {}

        Matrix operator() (Matrix& M) override
        {

            Matrix C = Matrix::Zero(1, M.cols());
            C.row(0).col(2).fill(1);
            return C;
        }
    };

    REGISTERED_GATE(TwoGate, "two");
}
