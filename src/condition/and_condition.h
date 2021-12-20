#ifndef NERU_AND_CONDITION_H
#define NERU_AND_CONDITION_H

#include "condition.h"

namespace Neru {

    class AndCondition : public Condition {
    public:
        // constructors
        AndCondition() = delete;
        AndCondition(std::shared_ptr<Condition> lhs, std::shared_ptr<Condition> rhs);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const AndCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;

    private:
        std::shared_ptr<Condition> _lhs, _rhs;
    };

}// namespace Neru

#endif//NERU_AND_CONDITION_H
