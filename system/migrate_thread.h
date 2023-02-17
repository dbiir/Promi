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
    RC process_send_migration(MigrationMessage* msg);
    RC process_recv_migration(MigrationMessage* msg);
    RC process_finish_migration(MigrationMessage* msg);
};

#endif