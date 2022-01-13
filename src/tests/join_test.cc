#include <cstdlib>
#include <memory>
#include "table/table.h"
#include "utils/utils.h"

using namespace Neru;

int get_field(const std::shared_ptr<Record> &rec, int idx){
    return int32_t(*std::static_pointer_cast<IntField const>(rec->get(idx)));
}

bool join_test() {
    std::cerr << "---Join---" << std::endl;
    // true
    std::cerr << File::init() << std::endl;

    Header head1 = {FieldType::INT, FieldType::INT}, head2 = {FieldType::INT, FieldType::INT, FieldType::INT};
    auto table1 = std::make_shared<Table>("table1", head1, true);
    auto table2 = std::make_shared<Table>("table2", head1, true);
    auto table3 = std::make_shared<Table>("table3", head2, true);
    std::vector<std::shared_ptr<Record>> records1, records2, records3;
    size_t len1 = 100, len2 = 100, len3 = 100;
    for (size_t i = 0; i < len1; ++i) {
        std::shared_ptr<Field> f1 = std::make_shared<IntField>(i);
        std::shared_ptr<Field> f2 = std::make_shared<IntField>(rand() % 3);
        records1.push_back(std::make_shared<Record>(std::vector{f1, f2}));
    }
    for (size_t i = 0; i < len2; ++i) {
        std::shared_ptr<Field> f1 = std::make_shared<IntField>(i);
        std::shared_ptr<Field> f2 = std::make_shared<IntField>(rand() % 3);
        records2.push_back(std::make_shared<Record>(std::vector{f1, f2}));
    }
    for (size_t i = 0; i < len3; ++i) {
        std::shared_ptr<Field> f1 = std::make_shared<IntField>(i);
        std::shared_ptr<Field> f2 = std::make_shared<IntField>(rand() % 3);
        std::shared_ptr<Field> f3 = std::make_shared<IntField>(rand() % 3);
        records3.push_back(std::make_shared<Record>(std::vector{f1, f2, f3}));
    }
    std::random_shuffle(records1.begin(), records1.end());
    std::random_shuffle(records2.begin(), records2.end());
    std::random_shuffle(records3.begin(), records3.end());
    for(auto &record: records1) table1->insert(record);
    for(auto &record: records2) table2->insert(record);
    for(auto &record: records3) table3->insert(record);

    Join join(std::vector<std::shared_ptr<Table>>{table1, table2, table3},
        std::vector<JoinCondition>{JoinCondition(table1, 1, table3, 1), JoinCondition(table2, 1, table3, 2)},
        std::vector<std::pair<std::shared_ptr<Table>, size_t>>{std::make_pair(table1, 0), std::make_pair(table2, 0), std::make_pair(table3, 0)});
    auto smj = join.SortMergeJoin()->select(), hj = join.HashJoin()->select();

    auto flag = std::make_unique<int[]>(len1 * len2 * len3);
    for(size_t i = 0; i < len1; ++i) for(size_t j = 0; j < len2; ++j) for(size_t k = 0; k < len3; ++k)
        if(get_field(records1[i], 1) == get_field(records3[k], 1) && get_field(records2[j], 1) == get_field(records3[k], 2))
            flag[(get_field(records1[i], 0) * len2 + get_field(records2[j], 0)) * len3 + get_field(records3[k], 0)] ^= 3;
    for(auto &record: smj)
        flag[(get_field(record, 0) * len2 + get_field(record,1)) * len3 + get_field(record, 2)] ^= 1;
    for(auto &record: hj)
        flag[(get_field(record, 0) * len2 + get_field(record,1)) * len3 + get_field(record, 2)] ^= 2;
    for(size_t i = 0; i < len1 * len2 * len3; i++)
        if(flag[i] != 0)
            return false;

    std::cerr << "Join passed." << std::endl;
    return true;
}
