#include "conditions.h"

namespace Neru {

    // operators
    std::ostream &operator<<(std::ostream &os, const Condition &cond) { return os << cond.info(); }

}// namespace Neru
