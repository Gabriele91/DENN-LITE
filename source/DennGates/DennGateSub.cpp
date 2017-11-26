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
            Matrix C = Matrix::Zero(1, A.cols());
            for (long j = 0, value = 0; j < A.size(); ++j, ++value)
            for (long i = 0; i < A.size(); ++i)
            {
                C(0, value) += A(0, i) * B(0, ((2 * j) + i) % max_int);
            }
            return C;
        }
    };
    REGISTERED_GATE(SubGate, "sub");
}
}