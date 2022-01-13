#include "LockTable.h"

namespace Neru {
    bool LockTable::request_lock(std::shared_ptr<Transaction> txn, Entry resource_id, LockType lock_type) {
        // TODO:each time a txn read/write a resource, it should acquire a lock on the resource
        // HINT: you shoud check 1. if entry exist in lock table
        //                       2. whether the resource is granted to the txn before
        //                       3. update the lock if this txn has a shared lock and request for exclusive lock
        //                       4. add the txn to the lock table, to check grant/wait
        //                       5. please organize the lock_set in the txn, it represent locks that this txn recently holds
        if(!lock_table_.count(resource_id))
            lock_table_[resource_id] = new txn_list();
        if(lock_type == LockType::SHARED){
            if(txn->has_exclusive_lock(resource_id) || txn->has_shared_lock(resource_id))
                return true;
            lock_table_[resource_id]->add(txn, lock_type, resource_id);
        }
        else{
            if(txn->has_exclusive_lock(resource_id))
                return true;
            if(txn->has_shared_lock(resource_id))
                return lock_table_[resource_id]->upgrade(txn, resource_id);
            lock_table_[resource_id]->add(txn, lock_type, resource_id);
        }
        return true;
    }

    bool LockTable::UnLock(std::shared_ptr<Transaction>txn, Entry resource_id) {
        // TODO: Because it's a strictly 2PL, we can only release lock when txn is committed/aborted
        // HINT: you shoud       1. remove the lock from the lock table
        //                       2. remove the lock from the txn
        //                       3. check whether should grant to other txn
        if(txn->get_state() != TransactionState::COMMITTED && txn->get_state() != TransactionState::ABORTED)
            return false;
        if(!lock_table_.count(resource_id))
            lock_table_[resource_id] = new txn_list();
        auto &txn_list = lock_table_[resource_id]->txn_list_;
        for(auto p = txn_list.begin(); p != txn_list.end(); p++)
            if(p->txn_id == txn->get_txn_id()){
                if(p->lock_type == LockType::SHARED)
                    txn->release_shared_lock(resource_id);
                else
                    txn->release_exclusive_lock(resource_id);
                txn_list.erase(p);
                break;
            }
        for(auto p = txn_list.begin(); p != txn_list.end(); p++)
            if(lock_table_[resource_id]->check_grant(p->lock_type))
                p->grant();
        return true;
    }

    bool LockTable::LockShared(std::shared_ptr<Transaction>txn, Entry resource_id) {
        return request_lock(txn, resource_id, LockType::SHARED);
    }

    bool LockTable::LockExclusive(std::shared_ptr<Transaction> txn, Entry resource_id) {
        return request_lock(txn, resource_id, LockType::EXCLUSIVE);
    }



}// namespace Neru