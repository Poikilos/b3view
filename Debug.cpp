#include "Debug.h"

using std::cout;
using std::endl;
using std::ostream;
using std::wcerr;
using std::wcout;

ostream& debug()
{
    std::flush(cout);
    return cout;
}
