#include "or_condition.h"

namespace Neru {

    // constructors
    OrCondition::OrCondition(std::shared_ptr<Condition> lhs, std::shared_ptr<Condition> rhs) : _lhs(lhs), _rhs(rhs) {}

    // info
    ConditionType OrCondition::type() const { return ConditionType::OR; }
    std::string OrCondition::info(size_t indent) const { return std::string(indent, '\t') + "OrCondition:\n" + _lhs->info(indent + 1) + "\n" + _rhs->info(indent + 1); }

    // operators
    std::ostream &operator<<(std::ostream &os, const OrCondition &cond) { return os << cond.info(); }

    // tools
    bool OrCondition::match(std::shared_ptr<Record> record) const { return _lhs->match(record) || _rhs->match(record); }

}// namespace Neru
