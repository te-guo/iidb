#include "page/data_page.h"
#include "utils/utils.h"

using namespace Neru;

bool data_page_test() {
    std::cerr << "---Data Page---" << std::endl;
    std::shared_ptr<uint8_t[]> pool(new uint8_t[12800]);
    std::bitset<FLAG_SIZE> bitset{114514};
    std::copy_n((uint8_t *) &bitset, sizeof(bitset), pool.get());
    Header head = {FieldType::INT, FieldType::DOUBLE};
    DataPage data_page1(head);
    //    data_page1.write(pool.get(), PAGE_SIZE, 0);
    //    std::cerr << data_page1._flag->to_ulong() << std::endl;
    std::vector<std::string> data1 = {"1", "2.3"};
    auto record1 = std::make_shared<Record>(data1, head);
    data_page1.set(114, record1);
    std::cerr << *data_page1.get(114) << std::endl;
    //    std::cerr << *data_page1.get(514) << std::endl;
    data_page1.read(pool.get(), PAGE_SIZE, 0);
    DataPage data_page2(head);
    data_page2.write(pool.get(), PAGE_SIZE, 0);
    std::cerr << *data_page1.get(114) << std::endl;
    return true;
}
