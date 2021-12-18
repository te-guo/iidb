#include "not_condition.h"

namespace Neru {

    // constructors
    NotCondition::NotCondition(std::shared_ptr<Condition> cond) : _cond(cond) {}

    // info
    ConditionType NotCondition::type() const { return ConditionType::NOT; }
    std::string NotCondition::info(size_t indent) const { return std::string(indent, '\t') + "NotCondition:\n" + _cond->info(indent + 1); }

    // operators
    std::ostream &operator<<(std::ostream &os, const NotCondition &cond) { return os << cond.info(); }

    // tools
    bool NotCondition::match(std::shared_ptr<Record> record) const { return !_cond->match(record); }

}// namespace Neru
