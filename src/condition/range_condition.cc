#include "range_condition.h"

namespace Neru {

    // constructors
    RangeCondition::RangeCondition(size_t lhs, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper) : _lhs(lhs), _upper(upper), _lower(lower) {}

    // info
    ConditionType RangeCondition::type() const { return ConditionType::RANGE; }
    std::string RangeCondition::info(size_t indent) const {
        return std::string(indent, '\t') + "RangeCondition:\n" + std::string(indent + 1, '\t') + "LHS: " + std::to_string(_lhs) + "\n" + std::string(indent + 1, '\t') + "RHS:\n" + std::string(indent + 2, '\t') + "Lower: " + _lower->info() + "\n" + std::string(indent + 2, '\t') + "Upper: " + _upper->info();
    }
    // operators
    std::ostream &operator<<(std::ostream &os, const RangeCondition &cond) { return os << cond.info(); }

    // tools
    bool RangeCondition::match(std::shared_ptr<Record> record) const { return *record->get(_lhs) >= *_lower && *record->get(_lhs) < *_upper; }

}// namespace Neru
