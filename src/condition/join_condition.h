#ifndef NERU_JOIN_CONDITION_H
#define NERU_JOIN_CONDITION_H

#include <memory>
#include "condition.h"

namespace Neru {

    class Table;

    class JoinCondition : public Condition {
    public:
        // constructors
        JoinCondition() = delete;
        JoinCondition(std::shared_ptr<Table> lhst, size_t lhs, std::shared_ptr<Table> rhst, size_t rhs);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const JoinCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;
        bool match(std::shared_ptr<Record> record1, std::shared_ptr<Record> record2) const;

    private:
        std::shared_ptr<Table> _lhst, _rhst;
        size_t _lhs, _rhs;
        friend class Join;
    };

}// namespace Neru

#endif//NERU_JOIN_CONDITION_H
