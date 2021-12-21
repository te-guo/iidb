#include "transaction/LockTable.h"
#include "transaction/TxManager.h"
#include "transaction/Txn.h"
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <assert.h>
using namespace Neru;

std::mutex mtx;
std::condition_variable cv;
bool flag_task1 = false;
bool flag_task2 = false;
bool flag2_task1 = false;
bool flag2_task2 = false;

void txn_read(std::shared_ptr<Transaction> txn, Table &table, std::vector<Entry> &entries, int idx, LockTable *lock_table, Record &ground_truth) {
    lock_table->LockShared(txn, entries[idx]);
    Record result_record;
    auto shared_lock = txn->get_shared_lock_set();
    bool deleted = false;
    bool result = txn->read(entries[idx], result_record, table, deleted);
    if (deleted) { return; }
    // verify results
    assert(result_record == ground_truth);
}

void txn_write(std::shared_ptr<Transaction> txn, Table &table, std::vector<Entry> &entries, int idx, LockTable *lock_table, WriteType type, Record &new_value) {
    lock_table->LockExclusive(txn, entries[idx]);
    txn->write(entries[idx], new_value, type, table);
}

// ==========TEST 1==========
//    Txn #0 | Txn #1 |
//    -----------------
//    BEGIN  |        |
//    W(X)   |        |
//    R(X)   |        |
//           | BEGIN  |
//           | R(X)   |
//    COMMIT |        |
//           | R(X)   |
//           | COMMIT |
//           |        |
//           |        |
//           |        |

void test1_task_1(TxManager &tx_manager, Table &table, std::vector<Entry> &entries, LockTable *lock_table) {
    // thread 1 will do this stuff
    auto txn0 = tx_manager.begin();
    std::cout << "txn0 begin" << std::endl;
    // sub process do write
    std::shared_ptr<Field> int_field = std::make_shared<IntField>(100);
    std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(99.99);
    auto record = std::make_shared<Record>(std::vector{int_field, double_field});
    std::thread t1(std::ref(txn_write), txn0, std::ref(table), std::ref(entries), 0, std::ref(lock_table), WriteType::UPDATE, std::ref(*record));
    std::cout << "txn0 write finish" << std::endl;
    // this sleep is only guaranteeing that t1 will be done before t2 begins
    sleep(1);

    // sub process do read
    std::thread t2(std::ref(txn_read), txn0, std::ref(table), std::ref(entries), 0, std::ref(lock_table), std::ref(*record));
    std::cout << "txn0 read finish" << std::endl;
    flag_task2 = true;
    cv.notify_one();

    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [] { return flag_task1; });
    flag_task1 = false;

    t1.join();
    t2.join();
    tx_manager.Commit(txn0);
    std::cout << "txn0 commit" << std::endl;
    flag_task2 = true;
    cv.notify_one();
}

void test1_task_2(TxManager &tx_manager, Table &table, std::vector<Entry> &entries, LockTable *lock_table) {
    // wait for task 1 to finish
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [] { return flag_task2; });
    flag_task2 = false;
    auto txn1 = tx_manager.begin();
    std::cout << "txn1 begin" << std::endl;
    Record result_record;

    // have a sub process to do this
    std::shared_ptr<Field> int_field = std::make_shared<IntField>(100);
    std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(99.99);
    auto ground_truth = std::make_shared<Record>(std::vector{int_field, double_field});
    std::thread t1(std::ref(txn_read), txn1, std::ref(table), std::ref(entries), 0, std::ref(lock_table), std::ref(*ground_truth));
    std::cout << "txn1 read finish 1" << std::endl;

    flag_task1 = true;
    cv.notify_one();
    cv.wait(lck, [] { return flag_task2; });
    flag_task2 = false;
    // sleep(2);
    // sub process do read
    std::thread t2(std::ref(txn_read), txn1, std::ref(table), std::ref(entries), 0, std::ref(lock_table), std::ref(*ground_truth));
    std::cout << "txn1 read finish 2" << std::endl;

    t1.join();
    t2.join();
    tx_manager.Commit(txn1);
    std::cout << "txn1 commit" << std::endl;
}

// ==========TEST 2==========
//    Txn #0 | Txn #1 |
//    -----------------
//    BEGIN  |        |
//           | BEGIN  |
//           | W(X)   |
//    R(X)   |        |
//           | R(X)   |
//           | ABORT  |
//    W(X)   |        |
//    R(X)   |        |
//    COMMIT |        |
//           |        |
//           |        |
void test2_task_1(TxManager &tx_manager, Table &table, std::vector<Entry> &entries, LockTable *lock_table) {
    // thread 1 will do this stuff

    auto txn0 = tx_manager.begin();
    flag2_task2 = true;
    std::cout << "txn0 begin" << std::endl;
    // wait for thread 2 to finish
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [] { return flag2_task1; });
    flag2_task1 = false;

    std::shared_ptr<Field> int_field = std::make_shared<IntField>(1);
    std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(116);
    auto ground_truth = std::make_shared<Record>(std::vector{int_field, double_field});
    std::thread t1(std::ref(txn_read), txn0, std::ref(table), std::ref(entries), 1, std::ref(lock_table), std::ref(*ground_truth));
    std::cout << "txn0 read finish" << std::endl;
    flag2_task2 = true;
    cv.notify_one();

    cv.wait(lck, [] { return flag2_task1; });
    flag2_task1 = false;

    // sub process do read
    std::thread t2(std::ref(txn_write), txn0, std::ref(table), std::ref(entries), 1, std::ref(lock_table), WriteType::DELETE, std::ref(*ground_truth));
    std::cout << "txn0 delete finish" << std::endl;

    sleep(1);
    std::thread t3(std::ref(txn_read), txn0, std::ref(table), std::ref(entries), 1, std::ref(lock_table), std::ref(*ground_truth));
    std::cout << "txn0 read finish" << std::endl;

    t1.join();
    t2.join();
    t3.join();
    tx_manager.Commit(txn0);
    std::cout << "txn0 commit" << std::endl;
}

void test2_task_2(TxManager &tx_manager, Table &table, std::vector<Entry> &entries, LockTable *lock_table) {
    // wait for task 1 to finish
    // this sleep is to prevent deadlock between two tasks
    sleep(1);
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [] { return flag2_task2; });
    flag2_task2 = false;

    auto txn1 = tx_manager.begin();
    std::cout << "txn1 begin" << std::endl;

    std::shared_ptr<Field> int_field = std::make_shared<IntField>(100);
    std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(99.99);
    auto record = std::make_shared<Record>(std::vector{int_field, double_field});
    std::thread t1(std::ref(txn_write), txn1, std::ref(table), std::ref(entries), 1, std::ref(lock_table), WriteType::UPDATE, std::ref(*record));
    std::cout << "txn1 write finish" << std::endl;

    flag2_task1 = true;
    cv.notify_one();
    cv.wait(lck, [] { return flag2_task2; });
    flag2_task2 = false;

    // sleep(2);
    // sub process do read
    std::thread t2(std::ref(txn_read), txn1, std::ref(table), std::ref(entries), 1, std::ref(lock_table), std::ref(*record));
    std::cout << "txn1 read finish" << std::endl;

    t1.join();
    t2.join();
    tx_manager.Abort(txn1);
    std::cout << "txn1 abort" << std::endl;
    flag2_task1 = true;
    cv.notify_one();
}


bool txn_test() {

    std::cout << "---Transaction---" << std::endl;
    Header head = {FieldType::INT, FieldType::DOUBLE};
    Entry::set(DATA_CAPACITY / utils::size(head));
    Table table("table", head, true);
    std::vector<Entry> entries;
    for (size_t i = 0; i < 3; ++i) {
        std::shared_ptr<Field> int_field = std::make_shared<IntField>(i);
        std::shared_ptr<Field> double_field = std::make_shared<DoubleField>(i * 2 + 114);
        auto record = std::make_shared<Record>(std::vector{int_field, double_field});
        entries.push_back(table.insert(record));
    }

    LockTable *lock_table = new LockTable();
    TxManager tx_manager(lock_table);
    {
        std::cout << "-------Test 1-------" << std::endl;
        std::thread t1(std::ref(test1_task_1), std::ref(tx_manager), std::ref(table), std::ref(entries), std::ref(lock_table));
        std::thread t2(std::ref(test1_task_2), std::ref(tx_manager), std::ref(table), std::ref(entries), std::ref(lock_table));
        t1.join();
        t2.join();
    }
    {

        std::cout << "-------Test 2-------" << std::endl;
        std::thread t1(std::ref(test2_task_1), std::ref(tx_manager), std::ref(table), std::ref(entries), std::ref(lock_table));
        std::thread t2(std::ref(test2_task_2), std::ref(tx_manager), std::ref(table), std::ref(entries), std::ref(lock_table));
        t1.join();
        t2.join();
    }


    delete lock_table;

    return true;
}

