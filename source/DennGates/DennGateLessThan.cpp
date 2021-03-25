#include "DennNRamGate.h"

namespace Denn
{
namespace NRam
{
    class LessThanGate : public Gate
    {
    public:

        LessThanGate() : Gate("lt", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M)  const override
        {
			Matrix::Index  max_int = M.cols();
            Matrix C = Matrix::Zero(1, max_int);
            for(Matrix::Index  i = 0; i < max_int; ++i)
            for(Matrix::Index  j = i + 1; j < max_int; ++j)
                    C(0, 1) += A(i, 0) * B(j, 0);
            C(0, 0) = 1 - C(0, 1);
            return C;
        }
    };
    REGISTERED_GATE(LessThanGate, "lt");
}
}