#include "migrate_thread.h"
#include "migmsg_queue.h"

#include "global.h"
#include "helper.h"
#include "manager.h"
#include "thread.h"
#include "io_thread.h"
#include "query.h"
#include "ycsb_query.h"
#include "tpcc_query.h"
#include "mem_alloc.h"
#include "transport.h"
#include "math.h"
#include "msg_thread.h"
#include "msg_queue.h"
#include "message.h"
#include "client_txn.h"
#include "work_queue.h"
#include "txn.h"
#include "ycsb.h"
#include "tpcc.h"
#include "index_btree.h"
#include "table.h"
#include "wl.h"

void MigrateThread::setup() {
    /*
    std::vector<Message*> * msgs;
	while(!simulation->is_setup_done()) {
		msgs = tport_man.recv_msg(get_thd_id());
		if (msgs == NULL) continue;
		while(!msgs->empty()) {
			Message * msg = msgs->front();
			if(msg->rtype == INIT_DONE) {
				printf("Received INIT_DONE from node %ld\n",msg->return_node_id);
				fflush(stdout);
				simulation->process_setup_msg();
			} else {
				assert(ISSERVER || ISREPLICA);
				//printf("Received Msg %d from node %ld\n",msg->rtype,msg->return_node_id);
                #if CC_ALG == CALVIN
			        if(msg->rtype == CALVIN_ACK ||(msg->rtype == CL_QRY && ISCLIENTN(msg->get_return_id())) ||
				    (msg->rtype == CL_QRY_O && ISCLIENTN(msg->get_return_id()))) {
				    work_queue.sequencer_enqueue(get_thd_id(),msg);
				    msgs->erase(msgs->begin());
				    continue;
			    }
			    if( msg->rtype == RDONE || msg->rtype == CL_QRY || msg->rtype == CL_QRY_O) {
				    assert(ISSERVERN(msg->get_return_id()));
				    work_queue.sched_enqueue(get_thd_id(),msg);
				    msgs->erase(msgs->begin());
				    continue;
			    }
                #endif
				work_queue.enqueue(get_thd_id(),msg,false);
			}
			msgs->erase(msgs->begin());
		}
		delete msgs;
	}
    */
	if (!ISCLIENT) {
		txn_man = (YCSBTxnManager *)
		    mem_allocator.align_alloc( sizeof(YCSBTxnManager));
		new(txn_man) YCSBTxnManager();
		// txn_man = (TxnManager*) malloc(sizeof(TxnManager));
		uint64_t thd_id = get_thd_id();
		txn_man->init(thd_id, _wl);
        txn_man->h_wl = (YCSBWorkload*)_wl;
	}    
}

TxnManager * MigrateThread::get_transaction_manager(MigrationMessage * msg) {
    #if CC_ALG == CALVIN
        TxnManager* local_txn_man =
        txn_table.get_transaction_manager(get_thd_id(), msg->get_txn_id(), msg->get_batch_id());
    #else
        TxnManager * local_txn_man = txn_table.get_transaction_manager(get_thd_id(),msg->get_txn_id(),0);
    #endif
    return local_txn_man;
}

RC MigrateThread::run(){
    tsetup();
    printf("Running MigrateThread %ld\n",_thd_id);

    //uint64_t ready_starttime;
    //uint64_t idle_starttime = 0;

    while(!simulation->is_done()) {
        txn_man = NULL;
        heartbeat();
        progress_stats();
        MigrationMessage* msg = new(MigrationMessage);
        //printf("begin receive message!\n");
        while(migmsg_queue.get_size()==0){
            if (simulation->is_done()) return RCOK;
        }
        //std::cout<<"the size is "<<migmsg_queue.get_size()<<endl;
        uint64_t dest = migmsg_queue.dequeue(get_thd_id(),msg);
        //std::cout<<"the size is "<<migmsg_queue.get_size()<<endl;

        assert(dest>=0);
        printf("get message111!\n");
        //if (msg->node_id_src != g_node_id) continue;
        printf("get message!\n");
        //std::cout<<"node_id_des is "<<msg->node_id_des<<endl;
        std::cout<<"message type is:"<<msg->get_rtype()<<" by migrate dequeue"<<endl;
        RC rc;
        txn_man = get_transaction_manager(msg);
        txn_man->register_thread(this);
        txn_man->h_wl = _wl;
        switch(msg->get_rtype()){
            case SEND_MIGRATION:
                rc = process_send_migration(msg);
                assert(rc==RCOK);
                break;
            case RECV_MIGRATION:
                rc = process_recv_migration(msg);
                assert(rc==RCOK);
                break;
            case FINISH_MIGRATION:
                rc = process_finish_migration(msg);
                assert(rc==RCOK);
                break;
            default:
                printf("message->rtpye is %d\n",msg->get_rtype());
                break;
        }
        delete msg;
    }
    return RCOK;
}

RC MigrateThread::process_send_migration(MigrationMessage* msg){
    DEBUG("SEND_MIGRATION %ld\n",msg->get_txn_id());
    std::cout<<"SEND_MIGRATION Time:"<<(get_sys_clock() - g_start_time) / BILLION<<endl;
    if (g_mig_starttime == 0) g_mig_starttime = get_sys_clock();


    RC rc =RCOK;
    start_time = get_sys_clock();
    txn_man->return_id = ((MigrationMessage*)msg)->return_node_id;
    txn_man->h_wl = _wl;
    
    ((MigrationMessage*)msg)->isdata = false;
    //std::cout<<&msg<<endl;
    std::cout<<"the size of msg is "<<((MigrationMessage*)msg)->get_size()<<endl;

    //update the migration metadata
    #if MIGRATION_ALG==DETEST
        update_part_map_status(((MigrationMessage*)msg)->part_id, 1);
        update_minipart_map_status(((MigrationMessage*)msg)->minipart_id, 1);
    #endif

    //init metadata
    uint64_t key_ptr = ((MigrationMessage*)msg)->key_start;//scan ptr
    int msg_num = 0;//msg max num
    msg_num = max(MSG_CHUNK_SIZE / MAX_TUPLE_SIZE - 15, 1);

    MigrationMessage* msg1 = new(MigrationMessage);
    *msg1 = *msg;  
    msg1->isdata = true;  
    msg1->islast = false;
    msg1->data_size = 0;
    msg1->key_start = key_ptr; 

    //fetch data and add into msg
    while(key_ptr <= ((MigrationMessage*)msg)->key_end){
        itemid_t* item = new(itemid_t);
        RC rc=WAIT;
        while (rc != RCOK){
            rc = ((YCSBWorkload*)_wl)->the_index->index_read(key_ptr,item,key_to_part(key_ptr),g_thread_cnt); 
        }

        row_t* row = new(row_t);
        row = ((row_t*)item->location);
        row_t* row_rtn = new(row_t);
        access_t access;
        #if MIGRATION_ALG == DETEST
            access = access_t::RD;
        #endif
        rc = txn_man->get_row(row,access,row_rtn);
        if (rc != RCOK){
            std::cout<<"trying to get lock..."<<endl;
        }
        while(rc != RCOK){
            rc = txn_man->get_row(row,access,row_rtn);
        }        

        msg1->isdata=true;
        msg1->data.emplace_back(*row_rtn);
        char tmp_char[MAX_TUPLE_SIZE];
        strcpy(tmp_char, row_rtn->get_data());
        string tmp_str = tmp_char;
        msg1->row_data.emplace_back(tmp_str);
        msg1->data_size ++;


        //send this msg and constrcut new msg
        if (msg1->data_size >= msg_num || key_ptr == ((MigrationMessage*)msg)->key_end){
            msg1->return_node_id = g_node_id;
            msg1->rtype = RECV_MIGRATION;
            msg1->key_end = key_ptr;
            if (key_ptr == ((MigrationMessage*)msg)->key_end){
                msg1->islast = true;
            }

            msg_queue.enqueue(get_thd_id(), msg1, msg1->node_id_des);
            std::cout<<"Send migration from "<<msg1->node_id_src<<"to "<<msg1->node_id_des<<" : enqueue finished "<<key_ptr<<endl;

            if (key_ptr == ((MigrationMessage*)msg)->key_end) break;

            MigrationMessage* msg1 = new(MigrationMessage);
            *msg1 = *msg;  
            msg1->isdata = true;  
            msg1->islast = false;     
            msg1->data_size = 0;   
            msg1->key_start = key_ptr + PART_CNT; 
        }

        key_ptr += PART_CNT;
    }

    //update migration metadata
    #if MIGRATION_ALG == DETEST
        update_minipart_map_status(((MigrationMessage*)msg)->minipart_id, 1);
        update_part_map_status(((MigrationMessage*)msg)->part_id, 1);
    #endif    

    return rc;
}

RC MigrateThread::process_recv_migration(MigrationMessage* msg){
    DEBUG("RECV_MIGRATION %ld\n",msg->get_txn_id());
    std::cout<<"RECV_MIGRATION Time:"<<(get_server_clock() - g_start_time) / BILLION<<endl;
    RC rc = RCOK;

    //receive data and construct
    uint64_t key_ptr;//received key
    key_ptr = ((MigrationMessage*)msg)->key_start;
    while (key_ptr != ((MigrationMessage*)msg)->key_end){
        row_t * new_row = NULL;
        uint64_t row_id;
        rc = this->_wl->tables["MAIN_TABLE"]->get_new_row(new_row, ((MigrationMessage*)msg)-> part_id, row_id);
        assert(rc == RCOK);
        uint64_t primary_key = key_ptr;
        new_row->set_primary_key(primary_key);

        #if SIM_FULL_ROW
            new_row->set_value(0, &primary_key,sizeof(uint64_t));
            Catalog * schema = this->_wl->tables["MAIN_TABLE"]->get_schema();
            for (UInt32 fid = 0; fid < schema->get_field_cnt(); fid ++) {
                char value[6] = "hello";
                new_row->set_value(fid, value,sizeof(value));				
            }
        #endif        

        itemid_t * m_item = (itemid_t *) mem_allocator.alloc( sizeof(itemid_t));
        assert(m_item != NULL);
        m_item->type = DT_row;
        m_item->location = new_row;
        m_item->valid = true;
        uint64_t idx_key = primary_key;
        rc = this->_wl->indexes["MAIN_INDEX"]->index_insert(idx_key, m_item, ((MigrationMessage*)msg)->part_id);
        assert(rc == RCOK);

        key_ptr += PART_CNT;
    }

    //update migration metadata 
    if (((MigrationMessage*)msg)->islast){
        update_minipart_map_status(((MigrationMessage*)msg)->minipart_id, 1);
        update_part_map_status(((MigrationMessage*)msg)->part_id, 1);
    }

    //send finish migration msg
    if (rc==RCOK){
        MigrationMessage * msg1 = new(MigrationMessage);
        *msg1 = *msg;
        msg1->rtype = FINISH_MIGRATION;
        #if MIGRATION_ALG == DETEST
            msg1->minipart_id = msg->minipart_id;
        #endif
        msg_queue.enqueue(get_thd_id(), msg1, msg1->node_id_src);
        std::cout<<"Recv migration from "<<msg1->node_id_src<<" to "<<msg1->node_id_des<<" : enqueue finished "<<endl<<"part is "<<msg1->part_id<<endl;
        std::cout<<(get_server_clock() - g_start_time) / BILLION<<endl;
        g_mig_endtime = get_sys_clock();
    }    

    return rc;
}

RC MigrateThread::process_finish_migration(MigrationMessage* msg){
    DEBUG("FINISH_MIGRATION %ld\n",msg->get_txn_id());
    std::cout<<"FINISH_MIGRATION Time:"<<(get_server_clock() - g_start_time) / BILLION<<endl;
    RC rc = RCOK;

    //update migration metadata
    #if MIGRATION_ALG == DETEST
        update_minipart_map(msg->minipart_id, msg->node_id_des);
        update_minipart_map_status(msg->minipart_id, 2);
        //sync msg to other nodes    
        for (uint64_t i=0; i< g_node_cnt + g_client_node_cnt; i++){
            if (i == g_node_id) continue;
            msg_queue.enqueue(get_thd_id(),Message::create_minipartmap_message(SET_PARTMAP, msg->part_id, msg->minipart_id, msg->node_id_des, 2), i);        
        }        
    #endif

    update_part_map(msg->part_id, msg->node_id_des);
    double migration_time = get_sys_clock() - start_time;
    std::cout<<"M Time:"<<migration_time / BILLION <<endl;
    txn_man->txn_stats.migration_time = migration_time;
    txn_man->commit();
    return rc;
}