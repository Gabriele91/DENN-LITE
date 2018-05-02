#include "DennNRamGate.h"

namespace Denn
{
namespace NRam
{
    class EqualityGate : public Gate
    {
    public:

        EqualityGate() : Gate("eq", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M)  const override
        {

			Matrix::Index  max_int = M.cols();
            Matrix C = Matrix::Zero(1, max_int);
            for (Matrix::Index  i = 0; i < max_int; ++i)
                C(0, 1) += A(i, 0) * B(i, 0);
            C(0, 0) = 1 - C(0, 1);
            return C;
        }

        int operator() (const int A, const int B, Matrix& M) const override
        {
            return int(A == B);
        }
    };
    REGISTERED_GATE(EqualityGate, "eq");
}
}