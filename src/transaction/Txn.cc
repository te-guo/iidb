#include "Txn.h"


namespace Neru {
    Transaction::Transaction(int txn_id) : txn_id(txn_id), state(TransactionState::GROWING), shared_lock{ std::unordered_set<Entry, Entry_hash>()}, exclusive_lock{ std::unordered_set<Entry, Entry_hash>()} {};
    Transaction::Transaction(const Transaction& other) : txn_id(other.txn_id), state(other.state), shared_lock{ std::unordered_set<Entry, Entry_hash>(other.shared_lock)}, exclusive_lock{ std::unordered_set<Entry, Entry_hash>(other.exclusive_lock)} {};

    void Transaction::fetch_shared_lock(Entry e) {
        this->shared_lock.insert(e);
    }
    void Transaction::release_shared_lock(Entry e) {
        this->shared_lock.erase(e);
    }
    bool Transaction::has_shared_lock(Entry e){
        return this->shared_lock.find(e) != this->shared_lock.end();
    }

    void Transaction::fetch_exclusive_lock(Entry e) {
        this->exclusive_lock.insert(e);
    }
    void Transaction::release_exclusive_lock(Entry e) {
        this->exclusive_lock.erase(e);
    }
    bool Transaction::has_exclusive_lock(Entry e){
        return this->exclusive_lock.find(e) != this->exclusive_lock.end();
    }

    int Transaction::get_txn_id() {
        return this->txn_id;
    }
    TransactionState Transaction::get_state() {
        return this->state;
    }
    void Transaction::set_state(TransactionState state) {
        this->state = state;
    }

    std::shared_ptr< std::unordered_multimap<Entry, Write_entry, Entry_hash, std::equal_to<Entry>> > Transaction::get_write_set() {
        return std::make_shared< std::unordered_multimap<Entry, Write_entry, Entry_hash, std::equal_to<Entry>>>(this->write_set);
    }
    std::unordered_set<Entry,Entry_hash> Transaction::get_shared_lock_set() {
        return this->shared_lock;
    }
    std::unordered_set<Entry,Entry_hash> Transaction::get_exclusive_lock_set() {
        return this->exclusive_lock;
    }

    bool Transaction::read(Entry e, Record &r, Table & table, bool &deleted) {
        deleted = false;
        if (this->has_shared_lock(e)||this->has_exclusive_lock(e)) {
            // first find if the entry is in the write set
            // if it is, then apply the changes to the entry
            // if it is not, then read the entry from table
            auto it = this->write_set.equal_range(e);
            if (it.first != it.second) {
                for (auto it2 = it.first; it2 != it.second; ++it2) {
                    // apply the changes to the entry
                    if(it2->second.type == WriteType::UPDATE){
                        r = it2->second.r;
                    }
                    else if(it2->second.type == WriteType::DELETE){
                        std::cout<<"Try to read a deleted record"<<std::endl;
                        deleted = true;
                        return true;
                    }
                    return true;
                }
            } else {
                r = *table.get(e);
                //std::cout<<"txn "<<this->txn_id<<" record "<<r<<std::endl;
                return true;
            }
        }
        throw std::runtime_error("Trying to read a record without a lock");
        return false;
        
    }

    bool Transaction::write(Entry e, Record& r, WriteType type, Table &table) {
        // append to the write set
        if (this->has_exclusive_lock(e)) {
            this->write_set.insert(std::make_pair(e, Write_entry(type,table, r)));
            return true;
        }
        else{
            throw std::runtime_error("Trying to write to a record that is not exclusivly locked");
            return false;
        }

    }

}// namespace Neru