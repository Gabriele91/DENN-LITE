#include "Config.h"
#include "DennNRamGate.h"
#include "DennDump.h"
#include <iostream>


namespace Denn
{
namespace NRam
{
    class AddGate : public Gate
    {
    public:

        AddGate() : Gate("add", BINARY) {}

        Matrix operator() (const Matrix& A, const Matrix& B, Matrix& M)  const override
        {
			Matrix::Index  max_int = M.cols();
            Matrix C = Matrix::Zero(1, A.cols());
            for (Matrix::Index  j = 0, value = 0; j < A.size(); ++j, ++value)
            for (Matrix::Index  i = 0; i < A.size(); ++i)
            {
                C(0, value) += A(0, i) * B(0, positive_mod(j - i, max_int));
            }
            return C;
        }
    };
    REGISTERED_GATE(AddGate, "add");
}
}