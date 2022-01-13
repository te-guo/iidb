#include "join_condition.h"
#include "table/table.h"

namespace Neru {

    // constructors
    JoinCondition::JoinCondition(std::shared_ptr<Table> lhst, size_t lhs, std::shared_ptr<Table> rhst, size_t rhs) : _lhst(lhst), _lhs(lhs), _rhst(rhst), _rhs(rhs) {}

    // info
    ConditionType JoinCondition::type() const { return ConditionType::JOIN; }
    std::string JoinCondition::info(size_t indent) const { return std::string(indent, '\t') + "JoinCondition:\n" + std::string(indent + 1, '\t') + "LHS: " + std::to_string(_lhs) + "\n" + std::string(indent + 1, '\t') + "RHS: " + std::to_string(_rhs); }

    // operators
    std::ostream &operator<<(std::ostream &os, const JoinCondition &cond) { return os << cond.info(); }

    // tools
    bool JoinCondition::match(std::shared_ptr<Record> record) const {
        std::shared_ptr<Record> record1 = std::make_shared<Record>(), record2 = std::make_shared<Record>();
        for(int i = 0; i < record->length(); i++)
            if(i < _lhst->type().size())
                record1->push((*record)[i]);
            else
                record2->push((*record)[i]);
        return match(record1, record2);
    }
    bool JoinCondition::match(std::shared_ptr<Record> record1, std::shared_ptr<Record> record2) const {
        return record1->match(_lhst->type()) && record2->match(_rhst->type()) && *record1->get(_lhs) == *record2->get(_rhs);
    }

}// namespace Neru
