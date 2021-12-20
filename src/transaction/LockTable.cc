#include "LockTable.h"

namespace Neru {
    bool LockTable::request_lock(std::shared_ptr<Transaction> txn, Entry resource_id, LockType lock_type) {
        // TODO:each time a txn read/write a resource, it should acquire a lock on the resource
        // HINT: you shoud check 1. if entry exist in lock table
        //                       2. whether the resource is granted to the txn before
        //                       3. update the lock if this txn has a shared lock and request for exclusive lock
        //                       4. add the txn to the lock table, to check grant/wait
        //                       5. please organize the lock_set in the txn, it represent locks that this txn recently holds
        
    }

    bool LockTable::UnLock(std::shared_ptr<Transaction>txn, Entry resource_id) {
        // TODO: Because it's a strictly 2PL, we can only release lock when txn is committed/aborted
        // HINT: you shoud       1. remove the lock from the lock table
        //                       2. remove the lock from the txn
        //                       3. check whether should grant to other txn


    }

    bool LockTable::LockShared(std::shared_ptr<Transaction>txn, Entry resource_id) {
        return request_lock(txn, resource_id, LockType::SHARED);
    }

    bool LockTable::LockExclusive(std::shared_ptr<Transaction> txn, Entry resource_id) {
        return request_lock(txn, resource_id, LockType::EXCLUSIVE);
    }



}// namespace Neru