#include "DennNRamGate.h"
#include "DennDump.h"


namespace Denn
{
namespace NRam
{
    class ReadGate : public Gate
    {
    public:

        ReadGate() : Gate("read",UNARY) {}

        Matrix operator() (const Matrix& Ptr, Matrix& M) override
        {
            return Ptr * M;
        }
    };

    REGISTERED_GATE(ReadGate, "read");
}
}