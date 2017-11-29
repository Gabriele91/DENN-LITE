#include "DennNRamGate.h"
#include "DennDump.h"

namespace Denn
{
namespace NRam
{
    class SubGate : public Gate
    {
    public:

        SubGate() : Gate("sub", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M)  const override
        {
			Matrix::Index max_int = M.cols();
            Matrix C = Matrix::Zero(1, max_int);
            for (Matrix::Index j = 0, value = 0; j < max_int; ++j, ++value)
            for (Matrix::Index i = 0; i < max_int; ++i)
            {
                C(0, value) += A(i, 0) * B(positive_mod((i - j), max_int), 0);
            }
            return C;
        }
    };
    REGISTERED_GATE(SubGate, "sub");
}
}