#include "and_condition.h"

namespace Neru {

    // constructors
    AndCondition::AndCondition(std::shared_ptr<Condition> lhs, std::shared_ptr<Condition> rhs) : _lhs(lhs), _rhs(rhs) {}

    // info
    ConditionType AndCondition::type() const { return ConditionType::AND; }
    std::string AndCondition::info(size_t indent) const { return std::string(indent, '\t') + "AndCondition:\n" + _lhs->info(indent + 1) + "\n" + _rhs->info(indent + 1); }

    // operators
    std::ostream &operator<<(std::ostream &os, const AndCondition &cond) { return os << cond.info(); }

    // tools
    bool AndCondition::match(std::shared_ptr<Record> record) const { return _lhs->match(record) && _rhs->match(record); }

}// namespace Neru
