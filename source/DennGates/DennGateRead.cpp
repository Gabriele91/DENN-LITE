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

        Matrix operator() (const Matrix& Ptr, Matrix& M)  const override
        {
            return (M.transpose() * Ptr).transpose();
        }

        int operator() (const int Ptr, Matrix& M) const override 
        {
            return M(0, Ptr);
        }
    };

    REGISTERED_GATE(ReadGate, "read");
}
}