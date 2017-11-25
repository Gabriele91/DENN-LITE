//
// Created by Valerio Belli on 15/11/17.
//
#include "DennNRamGate.h"
namespace Denn
{
namespace NRam
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
}