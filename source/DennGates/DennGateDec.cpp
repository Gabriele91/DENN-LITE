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
            return Denn::shift_top<Matrix>(A, 1).transpose();
        }

        int operator() (const int A, Matrix& M) const override 
        {
            return positive_mod(A - 1, int(M.cols()));
        }
    };

    REGISTERED_GATE(DecGate, "dec");
}
}