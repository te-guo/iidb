#pragma once
#include <atomic>
#include "Txn.h"
#include "LockTable.h"

namespace Neru {

class TxManager {
    public:
        TxManager(LockTable* lockTable);
        std::shared_ptr<Transaction> begin();
        void Commit(std::shared_ptr<Transaction> tx);
        void Abort(std::shared_ptr<Transaction> tx);
    private:
        std::atomic<int> nextTxId=0;
        LockTable* lockTable;
};

}// namespace Neru