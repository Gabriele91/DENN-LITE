#include "DennNRamGate.h"


namespace Denn
{
namespace NRam
{
    class MinGate : public Gate
    {
    public:

        MinGate() : Gate("min", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M)  const override
        {
            Matrix C = (*GateFactory::create("lt"))(A, B, M);
            return (C(0, 0) < C(0, 1) ? A : B).transpose();
        }

        int operator() (const int A, const int B, Matrix& M) const override 
        {
            return std::min(A, B);
        }
    };
    REGISTERED_GATE(MinGate, "min");
}
}