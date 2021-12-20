#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "../utils/entry.h"
#include "record/record.h"
#include "table/table.h"

namespace Neru{
struct Entry_hash{
    auto operator()(const Entry& e) const{
        return std::hash<std::size_t>()(e.page())*std::hash<std::size_t>()(e.slot());
    }
};
enum class WriteType{ UPDATE, DELETE };

struct Write_entry{
    WriteType type;
    Table &table;
    Record &r; // for update
    Write_entry( WriteType type, Table &table, Record &r):type(type),table(table),r(r){}

};

enum class TransactionState { COMMITTED, ABORTED, GROWING, SHRINKING };
class Transaction {
    // a transaction in database
    public:
    Transaction(Transaction const &);
    Transaction(int txn_id);

    // shared lock
    void fetch_shared_lock(Entry e);
    void release_shared_lock(Entry e);
    bool has_shared_lock(Entry e);

    // exclusive lock
    void fetch_exclusive_lock(Entry e);
    void release_exclusive_lock(Entry e);
    bool has_exclusive_lock(Entry e);

    // getter
    int get_txn_id();
    TransactionState get_state();
    void set_state(TransactionState state);
    std::shared_ptr< std::unordered_multimap<Entry, Write_entry, Entry_hash, std::equal_to<Entry>> >get_write_set();
    std::unordered_set<Entry,Entry_hash> get_shared_lock_set();
    std::unordered_set<Entry,Entry_hash> get_exclusive_lock_set();
    bool read(Entry e, Record &r, Table & table, bool &deleted);
    bool write(Entry e, Record & r, WriteType type, Table & table);



    private:
    int txn_id; // lsn of the transaction
    TransactionState state;
    std::vector<Entry> read_set;
    // for write set, we need to keep track of the original value for undo
    // mapping from entry to changed value, when commit, it will be written to disk
    std::unordered_multimap<Entry, Write_entry, Entry_hash, std::equal_to<Entry>> write_set;
    std::unordered_set<Entry,Entry_hash> shared_lock;
    std::unordered_set<Entry,Entry_hash> exclusive_lock;

};

}