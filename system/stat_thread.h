#include "thread.h"
#include "global.h"

using namespace std;

class StatThread : public Thread{
public:
  RC run();
  void setup();
  void analyze(vector<int> sch);
  clock_t now, lastCPU, lastSysCPU, lastUserCPU;
  struct tms timeSample;
  double percent;
};

