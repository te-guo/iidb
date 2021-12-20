#ifndef NERU_CONDITION_H
#define NERU_CONDITION_H

#include "record/record.h"

namespace Neru {

    enum class ConditionType { EQUAL,
                               RANGE,
                               NOT,
                               AND,
                               OR,
                               JOIN };

    class Condition {
    public:
        // info
        virtual ConditionType type() const = 0;
        virtual std::string info(size_t indent = 0) const = 0;

        // operators
        friend std::ostream &operator<<(std::ostream &os, const Condition &cond);

        // tools
        virtual bool match(std::shared_ptr<Record> record) const = 0;
    };

}// namespace Neru

#endif//NERU_CONDITION_H
