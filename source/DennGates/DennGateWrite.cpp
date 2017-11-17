#include "DennGate.h"
#include "DennDump.h"

namespace Denn
{
    class WriteGate : public Gate
    {
    public:

        WriteGate() : Gate("write", BINARY) {}

        Matrix operator() (const Matrix& Ptr, const Matrix& Val, Matrix& M) override
        {
            Matrix erase   = (Matrix::Ones(1, M.cols()) - Ptr).transpose() * Matrix::Ones(1, M.cols());
            Matrix contrib = Ptr.transpose() * Val;
            return erase.cwiseProduct(M) + contrib;
        }
    };
    REGISTERED_GATE(WriteGate, "write");
}