#include "DennNRamGate.h"

namespace Denn
{
namespace NRam
{
    class DecGate : public Gate
    {
    public:

        DecGate() : Gate("dec", UNARY) {}

        Matrix operator() (const Matrix& A, Matrix& M)  const override
        {
            return Denn::shift_top<Matrix>(A.transpose(), 1).transpose();
        }
    };

    REGISTERED_GATE(DecGate, "dec");
}
}