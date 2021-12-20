#ifndef NERU_NOT_CONDITION_H
#define NERU_NOT_CONDITION_H

#include "condition.h"

namespace Neru {

    class NotCondition : public Condition {
    public:
        // constructors
        NotCondition() = delete;
        NotCondition(std::shared_ptr<Condition> cond);

        // info
        virtual ConditionType type() const override;
        virtual std::string info(size_t indent = 0) const override;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const NotCondition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const override;


    private:
        std::shared_ptr<Condition> _cond;
    };

}// namespace Neru

#endif//NERU_NOT_CONDITION_H
