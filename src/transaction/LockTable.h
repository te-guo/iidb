#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <list>
#include "Txn.h"
#include <mutex>
#include <condition_variable>
#include <unordered_map>


namespace Neru{

enum class LockType {SHARED, EXCLUSIVE};
struct txn_item{
    int txn_id;
    std::mutex txn_mutex;
    std::condition_variable txn_cond;
    bool granted = false;
    LockType lock_type;
    txn_item(int txn_id, bool granted, LockType lock_type):
        txn_id(txn_id), granted(granted), lock_type(lock_type){}
    
    // change mutex and lock the resource
    void grant(){
        std::unique_lock<std::mutex> lock(txn_mutex);
        this->granted = true;
        this->txn_cond.notify_one(); // notify the txn that it can proceed
    }
    // if the transaction does not have the lock, they wait 
    bool wait(){
        std::unique_lock<std::mutex> lock(txn_mutex);
        txn_cond.wait(lock, [this]{return this->granted;});
        return true;
    }


};

struct txn_list{
    // the list of transactions that are waiting for the lock
    // have functions: 1. insert, 2. check grant, 3.
    std::list<txn_item> txn_list_;

    void add(std::shared_ptr<Transaction> txn, LockType lock_type, Entry entry){ 
        txn_list_.emplace_back(txn->get_txn_id(), false, lock_type);
        // if it can be grant 
        if(check_grant(lock_type)){
            txn_list_.back().grant();
            if (lock_type == LockType::SHARED){
                txn->fetch_shared_lock(entry);
            }
            else if (lock_type == LockType::EXCLUSIVE){
                txn->fetch_exclusive_lock(entry);
            }

        }
        else{
            // if it cannot be grant, wait
            txn_list_.back().wait();
            if (lock_type == LockType::SHARED){
                txn->fetch_shared_lock(entry);
            }
            else if (lock_type == LockType::EXCLUSIVE){
                txn->fetch_exclusive_lock(entry);
            }
        }
        
        
    }

    bool check_grant(LockType lock_type){
        // only if there is no other txn has thae lock, exclusive lock/shared lock can be granted
        // TODO: whether to grant the lock to this txn
        
        return false;
    }


};

class LockTable {
    // This is an implementation about the lock table in two phase locking(2PL) algorithm.
    // The lock table is a map from resource to a set of transactions.
    // For each transaction, we should know if it's granted or not, the transaction id and its lock type.
    // lock type: shared, exclusive.
    // when a txn hold a shared lock on a resource before, it can upgrade to exclusive lock.
    public:
        bool LockShared(std::shared_ptr<Transaction>txn, Entry resource_id);
        bool LockExclusive(std::shared_ptr<Transaction>txn, Entry resource_id);
        bool UnLock(std::shared_ptr<Transaction>txn, Entry resource_id);
 
    private:
        std::unordered_map<Entry, txn_list*, Entry_hash> lock_table_;
        bool request_lock(std::shared_ptr<Transaction> txn, Entry resource_id, LockType lock_type);
};

}// namespace Neru