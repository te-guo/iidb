#include "utils/utils.h"
#include <assert.h>

using namespace Neru;

size_t Entry::_capacity;

extern bool field_test();
extern bool record_test();
extern bool page_test();
extern bool data_page_test();
extern bool file_test();
extern bool entry_test();
extern bool data_file_test();
extern bool table_test();
extern bool index_test();
extern bool condition_test();
extern bool txn_test();


int main() {
    field_test();
    record_test();
    page_test();
    data_page_test();
    file_test();
    entry_test();
    std::cerr << data_file_test() << std::endl;
    std::cerr << table_test() << std::endl;
    std::cerr << index_test() << std::endl;
    condition_test();
    assert(txn_test() == 1);
    std::cerr << "---Exiting---" << std::endl;
    return 0;
}