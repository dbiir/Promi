#ifndef _MIGMSG_QUEUE_H_
#define _MIGMSG_QUEUE_H_

#include "global.h"
#include "helper.h"
#include "concurrentqueue.h"
#include "lock_free_queue.h"
#include <boost/lockfree/queue.hpp>
#include "semaphore.h"

class MigrationMessage;

struct migmsg_entry{
    MigrationMessage * msg;
    uint64_t dest;
    uint64_t starttime;
};

class MigrateMessageQueue{
public:
    void init();
    void statqueue(uint64_t thd_id, migmsg_entry * entry);
    void enqueue(uint64_t thd_id, MigrationMessage * msg, uint64_t dest);
    uint64_t dequeue(uint64_t thd_id, MigrationMessage *& msg);
    uint64_t get_size();
private:
    #if NETWORK_DELAY_TEST
        boost::lockfree::queue<msg_entry*> ** cl_m_queue;
    #endif
    boost::lockfree::queue<migmsg_entry*> ** m_queue;
    std::vector<migmsg_entry*> sthd_m_cache;
    uint64_t ** ctr;

    uint64_t msg_queue_size;
    sem_t 	_semaphore;
};


#endif