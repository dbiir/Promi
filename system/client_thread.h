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

#ifndef _CLIENT_THREAD_H_
#define _CLIENT_THREAD_H_

#include "global.h"
#include "helper.h"

class Workload;

class ClientThread : public Thread {
public:
	RC 			run();
  void setup();
  double caltime(vector<int> s);
  double callatency(vector<int> s);
  double caldis(vector<int> status);
  void assign(std::vector<std::vector<int> > plans, double theta);
  void assign1(std::vector<int64_t> vset, double theta);
private:
  myrand* mrand;
  uint64_t last_send_time;
  uint64_t send_interval;
};

#endif
