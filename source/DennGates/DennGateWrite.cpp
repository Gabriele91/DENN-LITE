#include "DennNRamGate.h"
#include "DennDump.h"

namespace Denn
{
namespace NRam
{
    class WriteGate : public Gate
    {
    public:

        WriteGate() : Gate("write", BINARY) {}

        Matrix operator() (const Matrix& Ptr, const Matrix& Val, Matrix& M)  const override
        {
            // Calculate the erase matrix with which the memory is erased
            Matrix erase = (RowVector::Ones(M.cols()) - Ptr.transpose()).transpose() * RowVector::Ones(M.cols());

            // Calculate where the new information will be added to the memory
            Matrix contrib = Ptr * Val.transpose();

            // Update M
            M = (erase.cwiseProduct(M) + contrib);

            // Return Value, as the paper says (i.e. a vector with P( x = 0 ) = 1.0)
            Matrix zero = Matrix::Zero(1, M.cols());
            zero(0, 0) = Scalar(1.0);
            return zero;
        }
    };
    REGISTERED_GATE(WriteGate, "write");
}
}