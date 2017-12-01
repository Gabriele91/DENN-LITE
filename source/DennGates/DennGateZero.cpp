//
// Created by Valerio Belli on 15/11/17.
//
#include "DennNRamGate.h"

namespace Denn
{
namespace NRam
{
    class ZeroGate : public Gate
    {
    public:

        ZeroGate() : Gate("zero", CONST) {}

        Matrix operator() (Matrix& M)  const override
        {
            Matrix C = Matrix::Zero(1, M.cols());
            C.row(0).col(0).fill(1);
            return C;
        }
    };

    REGISTERED_GATE(ZeroGate, "zero");
}
}