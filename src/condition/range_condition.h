#ifndef NERU_RANGE_CONDITION_H
#define NERU_RANGE_CONDITION_H

#include "condition.h"

namespace Neru {

    class RangeCondition : public Condition {
    public:
        // constructors
        RangeCondition() = delete;
        RangeCondition(size_t lhs, std::shared_ptr<Field> lower, std::shared_ptr<Field> upper);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const RangeCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;

    private:
        size_t _lhs;
        std::shared_ptr<Field> _lower, _upper;
    };

}// namespace Neru

#endif//NERU_RANGE_CONDITION_H
