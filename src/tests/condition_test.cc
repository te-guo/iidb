#include "condition/conditions.h"
#include "utils/utils.h"

using namespace Neru;

bool condition_test() {
    std::cerr << "---Condition---" << std::endl;
    auto equal_condition = std::make_shared<EqualCondition>(0, std::make_shared<IntField>(1));
    auto record1 = std::make_shared<Record>(std::vector<std::shared_ptr<Field>>({std::make_shared<IntField>(1), std::make_shared<DoubleField>("3.14")}));
    auto record2 = std::make_shared<Record>(std::vector<std::shared_ptr<Field>>({std::make_shared<IntField>(2), std::make_shared<DoubleField>("3.14")}));
    // true false
    std::cerr << std::boolalpha << equal_condition->match(record1) << ' ' << equal_condition->match(record2) << std::endl;
    //    std::cerr << *equal_condition << std::endl;
    auto range_condition = std::make_shared<RangeCondition>(0, std::make_shared<IntField>(2), std::make_shared<IntField>(3));
    // false true
    std::cerr << std::boolalpha << range_condition->match(record1) << ' ' << range_condition->match(record2) << std::endl;
    //    std::cerr << *range_condition << std::endl;
    auto and_condition = std::make_shared<AndCondition>(equal_condition, range_condition);
    // false false
    std::cerr << std::boolalpha << and_condition->match(record1) << ' ' << and_condition->match(record2) << std::endl;
    //    std::cerr << *and_condition << std::endl;
    auto not_condition = std::make_shared<NotCondition>(and_condition);
    // true true
    std::cerr << std::boolalpha << not_condition->match(record1) << ' ' << not_condition->match(record2) << std::endl;
    auto or_condition = std::make_shared<OrCondition>(and_condition, not_condition);
    std::cerr << *or_condition << std::endl;
    // true true
    std::cerr << std::boolalpha << or_condition->match(record1) << ' ' << or_condition->match(record2) << std::endl;
    return true;
}
