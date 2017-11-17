//
// Created by Valerio Belli on 15/11/17.
//

#include "DennGate.h"


namespace Denn
{
    class OneGate : public Gate
    {
    public:

        OneGate() : Gate("one", CONST) {}

        Matrix operator() (Matrix& M) override
        {
            Matrix C = Matrix::Zero(1, M.cols());
            C.row(0).col(1).fill(1);
            return C;
        }
    };

    REGISTERED_GATE(OneGate, "one");
}
