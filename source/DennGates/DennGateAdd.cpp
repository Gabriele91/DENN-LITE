#include "Config.h"
#include "DennGate.h"
#include "DennDump.h"
#include <iostream>

namespace Denn
{
    class AddGate : public Gate
    {
    public:

        AddGate() : Gate("add", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M) override
        {
            long max_int = M.cols();
            Matrix C = Matrix::Zero(1, A.cols());
            for (long j = 0, value = 0; j < A.size(); ++j, ++value)
                for (long i = 0; i < A.size(); ++i)
                {
                    C(0, value) += A(0, i) * B(0, positive_mod(j - i, max_int));
                }
            return C;
        }
    };
    REGISTERED_GATE(AddGate, "add");
}