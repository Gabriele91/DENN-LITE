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

			Matrix::Index  m_max_int = M.cols();
            Matrix C = Matrix::Zero(1, m_max_int);
            for (Matrix::Index  i = 0; i < A.cols(); ++i)
                C(0, 1) += A(0, i) * B(0, i);
            C(0, 0) = 1 - C(0, 1);
            return C;
        }
    };
    REGISTERED_GATE(EqualityGate, "eq");
}
}