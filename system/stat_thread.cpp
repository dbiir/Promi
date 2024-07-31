#include "stat_thread.h"


void StatThread::setup() {
   
}


RC StatThread::run(){
  tsetup();
  printf("Running StatThread %ld\n",_thd_id);

  now = times(&timeSample);
  lastCPU = now;
  lastSysCPU = timeSample.tms_stime;
  lastUserCPU = timeSample.tms_utime;

  uint64_t cnt;
  cnt = (get_sys_clock() - g_starttime) / BILLION;

  while(!simulation->is_done()) {
    while (((get_sys_clock() - g_starttime) / BILLION) >= cnt){
      now = times(&timeSample);
      if (now <= lastCPU || timeSample.tms_stime < lastSysCPU || timeSample.tms_utime < lastUserCPU) {
          //Overflow detection. Just skip this value.
          percent = -1.0;
      } else {
        percent = (timeSample.tms_stime - lastSysCPU) + (timeSample.tms_utime - lastUserCPU);
          percent /= (now - lastCPU);
          if(ISSERVER) {
            percent /= (g_total_thread_cnt);//numProcessors;
          } else if(ISCLIENT){
            percent /= (g_total_client_thread_cnt);//numProcessors;
          }
          percent *= 100;
      }
      percents[cnt++] = percent;
      lastCPU = now;
      lastSysCPU = timeSample.tms_stime;
      lastUserCPU = timeSample.tms_utime;
    }
  }
  return RCOK;
}