#include "TxManager.h"


namespace Neru{
    TxManager::TxManager(LockTable* lockTable)
    {
        this->lockTable = lockTable;
    }
    std::shared_ptr<Transaction> TxManager::begin(){
        return std::make_shared<Transaction>(nextTxId++);
    }
    void TxManager::Commit(std::shared_ptr<Transaction> txn){
        if(txn->get_state() == TransactionState::COMMITTED){
            throw std::runtime_error("Transaction already committed");
        }
        if(txn->get_state() == TransactionState::ABORTED){
            throw std::runtime_error("Transaction already aborted");
        }
        txn->set_state(TransactionState::COMMITTED);
        
        // the transaction will only write changes to its write set, and apply it to disk if it is committed
        // clear all stuff and write to disk
        auto write_set = *txn->get_write_set();
        for(auto& kv:write_set){
            
            if (kv.second.type == WriteType::DELETE){
                // delete
                kv.second.table.remove(kv.first);
            }
            else if (kv.second.type == WriteType::UPDATE){
                // update
                kv.second.table.update(kv.first, std::make_shared<Record>(kv.second.r));
            }
        }
        // release all the lock, if there exists
        for(auto shared_lock : (txn->get_shared_lock_set())){
            //std::cout<<"release shared lock"<<std::endl;
            lockTable->UnLock(txn, shared_lock);
        }
        for(auto exclusive_lock : (txn->get_exclusive_lock_set())){
            //std::cout<<"release exclusive lock"<<std::endl;
            lockTable->UnLock(txn, exclusive_lock);
        }

    }

    void TxManager::Abort(std::shared_ptr<Transaction> txn){
        txn->set_state(TransactionState::ABORTED);
        // release all the lock, if there exists
        for(auto shared_lock : (txn->get_shared_lock_set())){
            lockTable->UnLock(txn, shared_lock);
        }
        for(auto exclusive_lock : (txn->get_exclusive_lock_set())){
            lockTable->UnLock(txn, exclusive_lock);
        }
        
    }

}