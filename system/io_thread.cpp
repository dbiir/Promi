/*
	 Copyright 2016 Massachusetts Institute of Technology

	 Licensed under the Apache License, Version 2.0 (the "License");
	 you may not use this file except in compliance with the License.
	 You may obtain a copy of the License at

			 http://www.apache.org/licenses/LICENSE-2.0

	 Unless required by applicable law or agreed to in writing, software
	 distributed under the License is distributed on an "AS IS" BASIS,
	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 See the License for the specific language governing permissions and
	 limitations under the License.
*/

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

void InputThread::setup() {

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
	if (!ISCLIENT) {
		txn_man = (YCSBTxnManager *)
			mem_allocator.align_alloc( sizeof(YCSBTxnManager));
		new(txn_man) YCSBTxnManager();
		// txn_man = (TxnManager*) malloc(sizeof(TxnManager));
		uint64_t thd_id = get_thd_id();
		txn_man->init(thd_id, NULL);
	}
}

RC InputThread::run() {
	tsetup();
	printf("Running InputThread %ld\n",_thd_id);

	if(ISCLIENT) {
		client_recv_loop();
	} else {
		server_recv_loop();
	}

	return FINISH;

}

RC InputThread::client_recv_loop() {
	int rsp_cnts[g_servers_per_client];
	memset(rsp_cnts, 0, g_servers_per_client * sizeof(int));

	run_starttime = get_sys_clock();
	uint64_t return_node_offset;
	uint64_t inf;

	std::vector<Message*> * msgs;

	while (!simulation->is_done()) {
		heartbeat();
		uint64_t starttime = get_sys_clock();
		msgs = tport_man.recv_msg(get_thd_id());
		INC_STATS(_thd_id,mtx[28], get_sys_clock() - starttime);
		starttime = get_sys_clock();
		//while((m_query = work_queue.get_next_query(get_thd_id())) != NULL) {
		//Message * msg = work_queue.dequeue();
		if (msgs == NULL) continue;
		while(!msgs->empty()) {
			Message * msg = msgs->front();

			//update migration metadata
			#if MIGRATION
				switch(msg->get_rtype()){
					case SET_PARTMAP:{
						update_part_map(((SetPartMapMessage*)msg)->part_id, ((SetPartMapMessage*)msg)->node_id);
						update_part_map_status(((SetPartMapMessage*)msg)->part_id, ((SetPartMapMessage*)msg)->status);		
						

						std::cout<<"!!!!!!!!!!!partition "<<((SetPartMapMessage*)msg)->part_id<<" is on node "<<get_part_node_id(((SetPartMapMessage*)msg)->part_id)<<endl;
						std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
						
						break;
					}
					case SET_MINIPARTMAP:{
						#if MIGRATION
							#if MIGRATION_ALG == DETEST
								uint64_t node_src_id = get_minipart_node_id(((SetMiniPartMapMessage*)msg)->part_id, ((SetMiniPartMapMessage*)msg)->minipart_id);
								
								//debug
								//std::cout<<"node_src_id is "<<node_src_id<<endl;
								//std::cout<<"node_id is "<<((SetMiniPartMapMessage*)msg)->node_id<<endl;
								
								g_node_inflight_max_update(node_src_id, -(g_inflight_max / g_part_split_cnt));		
								g_node_inflight_max_update(((SetMiniPartMapMessage*)msg)->node_id, g_inflight_max / g_part_split_cnt);

								std::cout<<"g_node_inflight_max "<<g_node_inflight_max[node_src_id]<<" "<<g_node_inflight_max[((SetMiniPartMapMessage*)msg)->node_id]<<endl;
							#endif
						#endif


						update_minipart_map(
							((SetMiniPartMapMessage*)msg)->part_id, 
							((SetMiniPartMapMessage*)msg)->minipart_id, ((SetMiniPartMapMessage*)msg)->node_id);
						update_minipart_map_status(
							((SetMiniPartMapMessage*)msg)->part_id, 
							((SetMiniPartMapMessage*)msg)->minipart_id, ((SetMiniPartMapMessage*)msg)->status);

						std::cout<<"minipart "<<((SetMiniPartMapMessage*)msg)->minipart_id<<" is on node "<<
						get_minipart_node_id(
							((SetMiniPartMapMessage*)msg)->part_id,
							((SetMiniPartMapMessage*)msg)->minipart_id)
						<<endl;
						std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
						
						break;
					}
					default:
						break;
				}
			#endif


			//assert(msg->rtype == CL_RSP); 注释掉看看
			#if MIGRATION
				switch(msg->get_rtype()){
					case SET_REMUS: {
						std::cout<<((SetRemusMessage*)msg)->status<<endl;
						remus_status = ((SetRemusMessage*)msg)->status;
						update_remus_status(remus_status);
						std::cout<<"remus_stats is "<<remus_status<<endl;
						break;
					}
					case SET_PARTMAP:{
						//sleep(SYNCTIME);
						update_part_map(((SetPartMapMessage*)msg)->part_id, ((SetPartMapMessage*)msg)->node_id);
						//node_inflight_max[0] = 2 * MAX_TXN_IN_PART; //part迁移完成，修改inflght
						//node_inflight_max[1] = 2 * MAX_TXN_IN_PART;
						//std::cout<<"Time is"<<(get_sys_clock()-g_starttime)/BILLION<<endl;
						std::cout<<"!!!!!!!!!!!partition "<<((SetPartMapMessage*)msg)->part_id<<" is on node "<<GET_NODE_ID(((SetPartMapMessage*)msg)->part_id)<<endl;
						//std::cout<<"!!!!!!!!!!!partition 1 is on node "<<GET_NODE_ID(1)<<endl;
						std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
						//std::cout<<"!!!!!!!!!!!partition 2 is on node "<<GET_NODE_ID(2)<<endl;
						//std::cout<<"!!!!!!!!!!!partition 3 is on node "<<GET_NODE_ID(3)<<endl;
						/*
						//for tpcc we migrate more partitions
						#if MIGRATION
						#if WORKLOAD == TPCC
							uint64_t node_id_src = 0;
							uint64_t node_id_des = 1;
							uint64_t part_id = 
							Message * msg = Message::create_message(SEND_MIGRATION);
							((MigrationMessage*)msg)->node_id_src = node_id_src;
							((MigrationMessage*)msg)->node_id_des = node_id_des;
							((MigrationMessage*)msg)->part_id = part_id;
							((MigrationMessage*)msg)->minipart_id = 0;
							((MigrationMessage*)msg)->rtype = SEND_MIGRATION;
							((MigrationMessage*)msg)->data_size = g_dist_per_wh * g_cust_per_dist / PART_SPLIT_CNT;
							((MigrationMessage*)msg)->return_node_id = node_id_des;
							((MigrationMessage*)msg)->isdata = false;
							((MigrationMessage*)msg)->key_start =custKey(MIGRATION_PART+1, MIGRATION_PART+1, MIGRATION_PART+1);
							std::cout<<"msg size is:"<<msg->get_size()<<endl;
							std::cout<<"begin migration!"<<endl;
							std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
							msg_queue.enqueue(get_thd_id(),msg,node_id_src);
						#endif
						#endif
						*/
						break;
					}
					case SET_DETEST:{
						std::cout<<((SetDetestMessage*)msg)->status<<endl;
						detest_status = ((SetRemusMessage*)msg)->status;
						update_detest_status(detest_status);
						std::cout<<"detest_stats is "<<detest_status<<endl;
						break;
					}
					case SET_MINIPARTMAP:{
						update_minipart_map(((SetMiniPartMapMessage*)msg)->minipart_id, ((SetMiniPartMapMessage*)msg)->node_id);
						update_minipart_map_status(((SetMiniPartMapMessage*)msg)->minipart_id, ((SetMiniPartMapMessage*)msg)->status);

						//minipart迁移完成，修改inflght
						//node_inflight_max[0] = 2 * MAX_TXN_IN_PART + MAX_TXN_IN_PART * (((SetMiniPartMapMessage*)msg)->minipart_id+1) / DETEST_SPLIT; 
						//node_inflight_max[1] = 2 * MAX_TXN_IN_PART - MAX_TXN_IN_PART * (((SetMiniPartMapMessage*)msg)->minipart_id+1) / DETEST_SPLIT;
						std::cout<<"minipart "<<((SetMiniPartMapMessage*)msg)->minipart_id<<" is on node "<<get_minipart_node_id(((SetMiniPartMapMessage*)msg)->minipart_id)<<endl;
						std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
						/*
						if (((SetMiniPartMapMessage*)msg)->minipart_id < PART_SPLIT_CNT-1){//发送其他的minipart的迁移消息
							Message * msg1 = Message::create_message(SEND_MIGRATION);
							((MigrationMessage*)msg1)->node_id_src = 0;
							((MigrationMessage*)msg1)->node_id_des = 1;
							((MigrationMessage*)msg1)->part_id = 0;
							((MigrationMessage*)msg1)->minipart_id = ((SetMiniPartMapMessage*)msg)->minipart_id + 1;
							((MigrationMessage*)msg1)->rtype = SEND_MIGRATION;
							((MigrationMessage*)msg1)->data_size = g_synth_table_size / g_part_cnt / PART_SPLIT_CNT;
							((MigrationMessage*)msg1)->return_node_id = 1;
							((MigrationMessage*)msg1)->isdata = false;
							((MigrationMessage*)msg1)->key_start = ((MigrationMessage*)msg1)->minipart_id * (g_synth_table_size / PART_SPLIT_CNT) ;
							std::cout<<"msg size is:"<<msg1->get_size()<<endl;
							std::cout<<"begin migration!"<<endl;
							std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
							msg_queue.enqueue(get_thd_id(),msg1,0);
						}
						*/
						break;
					}
					case SET_SQUALL:{
						std::cout<<((SetSquallMessage*)msg)->status<<endl;
						squall_status = ((SetSquallMessage*)msg)->status;
						update_squall_status(squall_status);
						std::cout<<"squall_stats is "<<detest_status<<endl;
						break;
					}
					case SET_SQUALLPARTMAP:{
						update_squallpart_map(((SetSquallPartMapMessage*)msg)->squallpart_id, ((SetSquallPartMapMessage*)msg)->node_id);
						update_squallpart_map_status(((SetSquallPartMapMessage*)msg)->squallpart_id, ((SetSquallPartMapMessage*)msg)->status);

						//minipart迁移完成，修改inflght
						//node_inflight_max[0] = 2 * MAX_TXN_IN_PART + MAX_TXN_IN_PART * (((SetMiniPartMapMessage*)msg)->minipart_id+1) / DETEST_SPLIT; 
						//node_inflight_max[1] = 2 * MAX_TXN_IN_PART - MAX_TXN_IN_PART * (((SetMiniPartMapMessage*)msg)->minipart_id+1) / DETEST_SPLIT;
						std::cout<<"squallpart "<<((SetSquallPartMapMessage*)msg)->squallpart_id<<" is on node "<<get_squallpart_node_id(((SetSquallPartMapMessage*)msg)->squallpart_id)<<endl;
						std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
						break;
					}
					case SET_ROWMAP:{
						update_row_map_order(((SetRowMapMessage*)msg)->order, ((SetRowMapMessage*)msg)->node_id);
						update_row_map_status_order(((SetRowMapMessage*)msg)->order, ((SetRowMapMessage*)msg)->status);
						std::cout<<"cluster label "<<Order[((SetRowMapMessage*)msg)->order]<<" is on node "<<((SetRowMapMessage*)msg)->node_id<<endl;
						if (((SetRowMapMessage*)msg)->order < DETEST_SPLIT-1){//发送其他的label的迁移消息
							Message * msg1 = Message::create_message(SEND_MIGRATION);
							((MigrationMessage*)msg1)->node_id_src = 0;
							((MigrationMessage*)msg1)->node_id_des = 1;
							((MigrationMessage*)msg1)->order = ((SetRowMapMessage*)msg)->order + 1;
							((MigrationMessage*)msg1)->rtype = SEND_MIGRATION;
							((MigrationMessage*)msg1)->data_size = cluster_num[((SetRowMapMessage*)msg)->order + 1];
							((MigrationMessage*)msg1)->return_node_id = 1;
							((MigrationMessage*)msg1)->isdata = false;
							std::cout<<"msg size is:"<<msg1->get_size()<<endl;
							std::cout<<"begin migration!"<<endl;
							std::cout<<"Time is "<<(get_sys_clock() - run_starttime) / BILLION<<endl;
							msg_queue.enqueue(get_thd_id(),msg1,0);
						}
						break;
					}
					default:
						break;
				}
				
			#endif
		#if CC_ALG == BOCC || CC_ALG == FOCC || ONE_NODE_RECIEVE == 1
			return_node_offset = msg->return_node_id;
		#else
			return_node_offset = msg->return_node_id - g_server_start_node;
		#endif
			assert(return_node_offset < g_servers_per_client);
			rsp_cnts[return_node_offset]++;
			INC_STATS(get_thd_id(),txn_cnt,1);
			uint64_t timespan = get_sys_clock() - ((ClientResponseMessage*)msg)->client_startts;
			INC_STATS(get_thd_id(),txn_run_time, timespan);
			if (warmup_done) {
				INC_STATS_ARR(get_thd_id(),client_client_latency, timespan);
			}
			//INC_STATS_ARR(get_thd_id(),all_lat,timespan);
			inf = client_man.dec_inflight(return_node_offset);
			DEBUG("Recv %ld from %ld, %ld -- %f\n", ((ClientResponseMessage *)msg)->txn_id,
						msg->return_node_id, inf, float(timespan) / BILLION);
			assert(inf >=0);
			// delete message here
			msgs->erase(msgs->begin());
		}
		delete msgs;
		INC_STATS(_thd_id,mtx[29], get_sys_clock() - starttime);

	}

	printf("FINISH %ld:%ld\n",_node_id,_thd_id);
	fflush(stdout);
	return FINISH;
}


bool InputThread::fakeprocess(Message * msg) {
	RC rc __attribute__ ((unused));
	bool eq = false;

	txn_man->set_txn_id(msg->get_txn_id());
	// txn_table.get_transaction_manager(get_thd_id(),msg->get_txn_id(),0);
	// txn_man->txn_stats.clear_short();
	// if (CC_ALG != CALVIN) {
	//   txn_man->txn_stats.lat_network_time_start = msg->lat_network_time;
	//   txn_man->txn_stats.lat_other_time_start = msg->lat_other_time;
	// }
	// txn_man->txn_stats.msg_queue_time += msg->mq_time;
	// txn_man->txn_stats.msg_queue_time_short += msg->mq_time;
	// msg->mq_time = 0;
	// txn_man->txn_stats.work_queue_time += msg->wq_time;
	// txn_man->txn_stats.work_queue_time_short += msg->wq_time;
	// //txn_man->txn_stats.network_time += msg->ntwk_time;
	// msg->wq_time = 0;
	// txn_man->txn_stats.work_queue_cnt += 1;

	// DEBUG("%ld Processing %ld %d\n",get_thd_id(),msg->get_txn_id(),msg->get_rtype());
	// assert(msg->get_rtype() == CL_QRY || msg->get_txn_id() != UINT64_MAX);
	// uint64_t starttime = get_sys_clock();
		switch(msg->get_rtype()) {
			case RPREPARE:
				rc = RCOK;
				txn_man->set_rc(rc);
				msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,RACK_PREP),msg->return_node_id);
				break;
			case RQRY:
				rc = RCOK;
				txn_man->set_rc(rc);
				msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,RQRY_RSP),msg->return_node_id);
				break;
			case RQRY_CONT:
				rc = RCOK;
				txn_man->set_rc(rc);
				msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,RQRY_RSP),msg->return_node_id);
				break;
			case RFIN:
				rc = RCOK;
				txn_man->set_rc(rc);
				if(!((FinishMessage*)msg)->readonly || CC_ALG == MAAT || CC_ALG == OCC || CC_ALG == TICTOC || CC_ALG == BOCC || CC_ALG == SSI)
				// if(!((FinishMessage*)msg)->readonly || CC_ALG == MAAT || CC_ALG == OCC)
					msg_queue.enqueue(get_thd_id(),Message::create_message(txn_man,RACK_FIN),GET_TXN_NODE_ID(msg->get_txn_id()));
				// rc = process_rfin(msg);
				break;
			default:
				eq = true;
				break;
		}
		return eq;
	// uint64_t timespan = get_sys_clock() - starttime;
	// INC_STATS(get_thd_id(),worker_process_cnt,1);
	// INC_STATS(get_thd_id(),worker_process_time,timespan);
	// INC_STATS(get_thd_id(),worker_process_cnt_by_type[msg->rtype],1);
	// INC_STATS(get_thd_id(),worker_process_time_by_type[msg->rtype],timespan);
	// DEBUG("%ld EndProcessing %d %ld\n",get_thd_id(),msg->get_rtype(),msg->get_txn_id());
}

RC InputThread::server_recv_loop() {

	myrand rdm;
	rdm.init(get_thd_id());
	RC rc = RCOK;
	assert (rc == RCOK);
	uint64_t starttime;

	std::vector<Message*> * msgs;
	while (!simulation->is_done()) {
		heartbeat();
		starttime = get_sys_clock();

		msgs = tport_man.recv_msg(get_thd_id());

		INC_STATS(_thd_id,mtx[28], get_sys_clock() - starttime);
		starttime = get_sys_clock();

		if (msgs == NULL) continue;
		while(!msgs->empty()) {
			Message * msg = msgs->front();
			if(msg->rtype == INIT_DONE) {
				msgs->erase(msgs->begin());
				continue;
			}
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
#ifdef FAKE_PROCESS
			if (fakeprocess(msg))
#endif
<<<<<<< HEAD
=======
			//if (msg->rtype == SEND_MIGRATION) std::cout<<"get SEND"<<endl;
>>>>>>> 8ee691f8bc5012b01a09fa4ed4cd44586f4b7b9d
			//if (msg->rtype == RECV_MIGRATION) std::cout<<"get RECV"<<endl;
			work_queue.enqueue(get_thd_id(),msg,false);
			msgs->erase(msgs->begin());
		}
		delete msgs;
		INC_STATS(_thd_id,mtx[29], get_sys_clock() - starttime);

	}
	printf("FINISH %ld:%ld\n",_node_id,_thd_id);
	fflush(stdout);
	return FINISH;
}

void OutputThread::setup() {
	DEBUG_M("OutputThread::setup MessageThread alloc\n");
	messager = (MessageThread *) mem_allocator.alloc(sizeof(MessageThread));
	messager->init(_thd_id);
	while (!simulation->is_setup_done()) {
		messager->run();
	}
}

RC OutputThread::run() {

	tsetup();
	printf("Running OutputThread %ld\n",_thd_id);

	while (!simulation->is_done()) {
		heartbeat();
		messager->run();
	}

	printf("FINISH %ld:%ld\n",_node_id,_thd_id);
	fflush(stdout);
	return FINISH;
}


