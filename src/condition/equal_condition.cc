#include "equal_condition.h"

namespace Neru {

    // constructors
    EqualCondition::EqualCondition(size_t lhs, std::shared_ptr<Field> rhs) : _lhs(lhs), _rhs(rhs) {}

    // info
    ConditionType EqualCondition::type() const { return ConditionType::EQUAL; }
    std::string EqualCondition::info(size_t indent) const { return std::string(indent, '\t') + "EqualCondition:\n" + std::string(indent + 1, '\t') + "LHS: " + std::to_string(_lhs) + "\n" + std::string(indent + 1, '\t') + "RHS: " + _rhs->info(); }

    // operators
    std::ostream &operator<<(std::ostream &os, const EqualCondition &cond) { return os << cond.info(); }

    // tools
    bool EqualCondition::match(std::shared_ptr<Record> record) const { return *record->get(_lhs) == *_rhs; }

}// namespace Neru
