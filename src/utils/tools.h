#ifndef NERU_TOOLS_H
#define NERU_TOOLS_H

#include "field/fields.h"
#include "record/record.h"

#include <iostream>
#include <vector>

namespace Neru {

    namespace utils {

        // FieldType
        FieldType from(std::string data);
        std::string info(const FieldType &type);

        // Header
        Header from(std::vector<std::string> data);
        size_t size(const Header &head);
        std::string info(const Header &head);

    }// namespace utils

    // operators
    std::ostream &operator<<(std::ostream &os, const FieldType &type);
    std::ostream &operator<<(std::ostream &os, const Header &head);

}// namespace Neru

#endif//NERU_TOOLS_H
