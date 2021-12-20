#include "index/index.h"
#include "utils/utils.h"

using namespace Neru;

bool index_test() {
    std::cerr << "---Index---" << std::endl;
    Index index;
    for (size_t i = 0; i < 514; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        index.insert(int_field, {0, i});
    }
    for (size_t i = 0; i < 514; ++i) {
        Entry entry = index.at(std::make_shared<IntField>(i));
        if (entry.slot() != i)
            return false;
    }
    std::cerr << "At passed." << std::endl;
    auto entries = index.range(std::make_shared<IntField>(114), std::make_shared<IntField>(411));
    std::cerr << entries.size() << std::endl;
    bool flag[entries.size()];
    for (size_t i = 0; i < entries.size(); ++i)
        flag[i] = false;
    for (auto entry : entries)
        flag[entry.slot() - 114] = true;
    for (size_t i = 0; i < entries.size(); ++i)
        if (!flag[i])
            return false;
    std::cerr << "Range passed." << std::endl;
    index.insert(std::make_shared<IntField>(114), {514, 0});
    std::cerr << index.at(std::make_shared<IntField>(114)) << std::endl;
    // false true
    std::cerr << std::boolalpha << index.erase(std::make_shared<IntField>(1919)) << ' ' << index.erase(std::make_shared<IntField>(114)) << std::endl;
    // false
    std::cerr << std::boolalpha << index.has(std::make_shared<IntField>(114)) << std::endl;
    return true;
}
