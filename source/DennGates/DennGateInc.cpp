#include "DennNRamGate.h"
#include "DennDump.h"

namespace Denn
{
namespace NRam
{
    class IncGate : public Gate
    {
    public:

        IncGate() : Gate("inc",UNARY) {}

        Matrix operator() (const Matrix& A, Matrix& M)  const override
        {
            return Denn::shift_bottom<Matrix>(A, 1).transpose();
        }

        int operator() (const int A, Matrix& M) const override 
        {
            return positive_mod(A + 1, int(M.cols()));
        }
    };

    REGISTERED_GATE(IncGate, "inc");
}
}