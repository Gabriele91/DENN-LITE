#include "DennGate.h"
#include "DennDump.h"

namespace Denn
{
    class IncGate : public Gate
    {
    public:

        IncGate() : Gate("inc",UNARY) {}

        Matrix operator() (const Matrix& A, Matrix& M) override
        {
            return Denn::shift_bottom<Matrix>(A.transpose(), 1).transpose();
        }
    };

    REGISTERED_GATE(IncGate, "inc");
}