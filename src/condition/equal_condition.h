#ifndef NERU_EQUAL_CONDITION_H
#define NERU_EQUAL_CONDITION_H

#include "condition.h"

namespace Neru {

    class EqualCondition : public Condition {
    public:
        // constructors
        EqualCondition() = delete;
        EqualCondition(size_t lhs, std::shared_ptr<Field> rhs);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const EqualCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;

    private:
        size_t _lhs;
        std::shared_ptr<Field> _rhs;
    };

}// namespace Neru

#endif//NERU_EQUAL_CONDITION_H
