#ifndef NERU_OR_CONDITION_H
#define NERU_OR_CONDITION_H

#include "condition.h"

namespace Neru {

    class OrCondition : public Condition {
    public:
        // constructors
        OrCondition() = delete;
        OrCondition(std::shared_ptr<Condition> lhs, std::shared_ptr<Condition> rhs);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const OrCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;

    private:
        std::shared_ptr<Condition> _lhs, _rhs;
    };

}// namespace Neru

#endif//NERU_OR_CONDITION_H
