#include "DennGate.h"

namespace Denn
{
    class LessThanGate : public Gate
    {
    public:

        LessThanGate() : Gate("lt", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M) override
        {
            long m_max_int = M.cols();
            Matrix C = Matrix::Zero(1, m_max_int);
            for (size_t i = 0; i < A.cols(); ++i)
                for(size_t j = i + 1; j < A.cols(); ++j)
                    C(0, 1) += A(0, i) * B(0, j);
            C(0, 0) = 1 - C(0, 1);
            return C;
        }
    };
    REGISTERED_GATE(LessThanGate, "lt");
}