#ifndef _MIGMSG_QUEUE_H_
#define _MIGMSG_QUEUE_H_

#include "global.h"
#include "helper.h"
#include "concurrentqueue.h"
#include "lock_free_queue.h"
#include <boost/lockfree/queue.hpp>
#include "semaphore.h"

class Message;

struct migmsg_entry{
    Message * msg;
    uint64_t dest;
    uint64_t starttime;
};

class MigrateMessageQueue{
public:
    void init();
    void statqueue(uint64_t thd_id, migmsg_entry * entry);
    void enqueue(uint64_t thd_id, Message * msg, uint64_t dest);
    uint64_t dequeue(uint64_t thd_id, Message *& msg);
    uint64_t get_size();
private:
    boost::lockfree::queue<migmsg_entry*> ** m_queue;
    std::vector<migmsg_entry*> sthd_m_cache;
    uint64_t ** ctr;

    uint64_t msg_queue_size;
    sem_t 	_semaphore;
};


#endif