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
            return Denn::shift_bottom<Matrix>(A.transpose(), 1).transpose();
        }
    };

    REGISTERED_GATE(IncGate, "inc");
}
}