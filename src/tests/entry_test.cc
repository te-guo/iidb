#include "page/data_page.h"
#include "utils/utils.h"

using namespace Neru;

bool entry_test() {
    std::cerr << "---Entry---" << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    Entry entry1(0, 2);
    std::cerr << entry1 << std::endl;
    std::cerr << ++entry1 << std::endl;
    std::cerr << --entry1 << std::endl;
    std::cerr << (entry1 += 2) << std::endl;
    std::cerr << (entry1 += 114514) << std::endl;
    std::cerr << (entry1 -= 114) << std::endl;
    //    std::cerr << (entry1 -= 114514) << std::endl;
    Entry entry2(entry1);
    // true false false
    std::cerr << (entry1 == entry2) << ' ' << (entry1 != entry2) << ' ' << (entry1 < entry2) << std::endl;
    ++entry2;
    // false true true
    std::cerr << (entry1 == entry2) << ' ' << (entry1 != entry2) << ' ' << (entry1 < entry2) << std::endl;
    return true;
}
