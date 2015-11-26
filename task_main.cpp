#include <unistd.h>
#include <chrono>
#include <pthread.h>

#include "task.h"


namespace NRTSimulator {
	void setRealtimePriority() {
		struct sched_param param;
		param.sched_priority = 80;
		sched_setscheduler(0, SCHED_FIFO, &param);
	}
}
int main(int argc, char *argv[])
{
	NRTSimulator::setRealtimePriority();
	

	long long executionTime (50 * 1000 * 1000);
	long long period (20 * 1000 * 1000);
	long long offset = std::chrono::duration_cast<std::chrono::nanoseconds>(
			(std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();

	NRTSimulator::TTask task(executionTime, period, offset, 10.5);
	task.Run();
}
