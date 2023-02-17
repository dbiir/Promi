#include "migmsg_queue.h"
#include "mem_alloc.h"
#include "query.h"
#include "pool.h"
#include "message.h"
#include <boost/lockfree/queue.hpp>

void MigrateMessageQueue::init(){
    m_queue = new boost::lockfree::queue<migmsg_entry* > * [g_migrate_thread_cnt];
    #if NETWORK_DELAY_TEST
        cl_m_queue = new boost::lockfree::queue<msg_entry* > * [g_this_send_thread_cnt];
    #endif
    for(uint64_t i = 0; i < g_migrate_thread_cnt; i++) {
        m_queue[i] = new boost::lockfree::queue<migmsg_entry* > (0);
        #if NETWORK_DELAY_TEST
            cl_m_queue[i] = new boost::lockfree::queue<migmsg_entry* > (0);
        #endif
    }
    ctr = new  uint64_t * [g_migrate_thread_cnt];
    for(uint64_t i = 0; i < g_migrate_thread_cnt; i++) {
        ctr[i] = (uint64_t*) mem_allocator.align_alloc(sizeof(uint64_t));
        *ctr[i] = i % g_thread_cnt;
    }
    msg_queue_size=0;
    sem_init(&_semaphore, 0, 1);
    for (uint64_t i = 0; i < g_migrate_thread_cnt; i++) sthd_m_cache.push_back(NULL);   
}

void MigrateMessageQueue::statqueue(uint64_t thd_id, migmsg_entry * entry){//不确定要不要改
    Message *msg = entry->msg;
    if (msg->rtype == CL_QRY || msg->rtype == CL_QRY_O || msg->rtype == RTXN_CONT ||
    msg->rtype == RQRY_RSP || msg->rtype == RACK_PREP  ||
    msg->rtype == RACK_FIN || msg->rtype == RTXN) {
        // these msg will send back to local node
        uint64_t queue_time = get_sys_clock() - entry->starttime;
		INC_STATS(thd_id,trans_msg_remote_wait,queue_time);
    } else if (msg->rtype == RQRY || msg->rtype == RQRY_CONT ||
    msg->rtype == RFIN || msg->rtype == RPREPARE ||
    msg->rtype == RFWD){
        // these msg will send to remote node
        uint64_t queue_time = get_sys_clock() - entry->starttime;
		INC_STATS(thd_id,trans_msg_local_wait,queue_time);
    } else if (msg->rtype == CL_RSP) {
        uint64_t queue_time = get_sys_clock() - entry->starttime;
		INC_STATS(thd_id,trans_return_client_wait,queue_time);
    } 
}

void MigrateMessageQueue::enqueue(uint64_t thd_id, Message * msg, uint64_t dest){
    DEBUG("MQ Enqueue %ld\n",dest)
    assert(dest < g_total_node_cnt);
    #if ONE_NODE_RECIEVE == 1 && defined(NO_REMOTE) && LESS_DIS_NUM == 10
    #else
        //assert(dest != g_node_id); 为了迁移mig把这里注释掉，允许自己发给自己
    #endif
    DEBUG_M("MessageQueue::enqueue msg_entry alloc\n");
    migmsg_entry * entry = (migmsg_entry*) mem_allocator.alloc(sizeof(struct migmsg_entry));
    //msg_pool.get(entry);
    entry->msg = msg;
    entry->dest = dest;
    entry->starttime = get_sys_clock();
    assert(entry->dest < g_total_node_cnt);
    uint64_t mtx_time_start = get_sys_clock();
    #if CC_ALG == CALVIN
        // Need to have strict message ordering for sequencer thread
        uint64_t rand = thd_id % g_this_send_thread_cnt;
    #elif WORKLOAD == DA
    uint64_t rand = 0;
    #else
    uint64_t rand = mtx_time_start % g_migrate_thread_cnt;
    #endif
    #if NETWORK_DELAY_TEST
        if(ISCLIENTN(dest)) {
            while (!cl_m_queue[rand]->push(entry) && !simulation->is_done()) {
        }
        return;
    }
    #endif
    while (!m_queue[rand]->push(entry) && !simulation->is_done()) {
    }
    INC_STATS(thd_id,mtx[3],get_sys_clock() - mtx_time_start);
    INC_STATS(thd_id,migmsg_queue_enq_cnt,1);
    sem_wait(&_semaphore);
    msg_queue_size++;
    sem_post(&_semaphore);
    INC_STATS(thd_id,trans_migmsg_queue_item_total,msg_queue_size);
}

uint64_t MigrateMessageQueue::dequeue(uint64_t thd_id, Message *& msg){

    migmsg_entry * entry = NULL;
    uint64_t dest = UINT64_MAX;
    uint64_t mtx_time_start = get_sys_clock();
    bool valid = false;
    #if NETWORK_DELAY_TEST
        valid = cl_m_queue[thd_id%g_this_send_thread_cnt]->pop(entry);
        if(!valid) {
            entry = sthd_m_cache[thd_id % g_this_send_thread_cnt];
            if(entry)
                valid = true;
            else
                valid = m_queue[thd_id%g_this_send_thread_cnt]->pop(entry);
        }
    #elif WORKLOAD == DA
    valid = m_queue[0]->pop(entry);
    #else
    //uint64_t ctr_id = thd_id % g_this_send_thread_cnt;
    //uint64_t start_ctr = *ctr[ctr_id];
    valid = m_queue[thd_id % g_migrate_thread_cnt]->pop(entry);
    #endif
    /*
    while(!valid && !simulation->is_done()) {
        ++(*ctr[ctr_id]);
        if(*ctr[ctr_id] >= g_this_thread_cnt)
        *ctr[ctr_id] = 0;
        valid = m_queue[*ctr[ctr_id]]->pop(entry);
        if(*ctr[ctr_id] == start_ctr)
        break;
    }
    */
    INC_STATS(thd_id,mtx[4],get_sys_clock() - mtx_time_start);
    uint64_t curr_time = get_sys_clock();
    if(valid) {
        assert(entry);
        #if NETWORK_DELAY_TEST
            if(!ISCLIENTN(entry->dest)) {
                if(ISSERVER && (get_sys_clock() - entry->starttime) < g_network_delay) {
                    sthd_m_cache[thd_id%g_this_send_thread_cnt] = entry;
                    INC_STATS(thd_id,mtx[5],get_sys_clock() - curr_time);
                    return UINT64_MAX;
                } else {
                    sthd_m_cache[thd_id%g_this_send_thread_cnt] = NULL;
                }
                if(ISSERVER) {
                    INC_STATS(thd_id,mtx[38],1);
                    INC_STATS(thd_id,mtx[39],curr_time - entry->starttime);
                }
            }
        #endif
        dest = entry->dest;
        assert(dest < g_total_node_cnt);
        msg = entry->msg;
        DEBUG("MQ Dequeue %ld\n",dest)
        statqueue(thd_id, entry);
        INC_STATS(thd_id,migmsg_queue_delay_time,curr_time - entry->starttime);
        INC_STATS(thd_id,migmsg_queue_cnt,1);
        msg->mq_time = curr_time - entry->starttime;
        //msg_pool.put(entry);
        DEBUG_M("MessageQueue::enqueue msg_entry free\n");
        mem_allocator.free(entry,sizeof(struct migmsg_entry));
        sem_wait(&_semaphore);
        msg_queue_size--;
        sem_post(&_semaphore);
    } else {
        msg = NULL;
        dest = UINT64_MAX;
    }
    INC_STATS(thd_id,mtx[5],get_sys_clock() - curr_time);
    return dest;
}

uint64_t MigrateMessageQueue::get_size(){
    return this->msg_queue_size;
}