#include "DennNRamGate.h"

namespace Denn
{
namespace NRam
{
    class MaxGate : public Gate
    {
    public:

        MaxGate() : Gate("max", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M) override
        {
            Matrix C = (*GateFactory::create("lt"))(A, B, M);
            return C(0, 0) < C(0, 1) ? B : A;
        }
    };
    REGISTERED_GATE(MaxGate, "max");
}
}