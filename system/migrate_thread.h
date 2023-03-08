#ifndef _MIGTHREAD_H_
#define _MIGTHREAD_H_

#include "global.h"
#include "message.h"
#include "helper.h"
#include "ycsb.h"
#include "row.h"
#include "thread.h"

class Workload;

class MigrateThread : public Thread{
public:
    RC run();
    void setup();
    TxnManager * txn_man;
    TxnManager * get_transaction_manager(MigrationMessage * msg);
    RC process_send_migration(MigrationMessage* msg);
    RC process_recv_migration(MigrationMessage* msg);
    RC process_finish_migration(MigrationMessage* msg);
    uint64_t start_time;
    int miss_cnt=0;
};

#endif