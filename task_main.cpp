#include <unistd.h>
#include <chrono>
#include <pthread.h>
#include <iostream>

#include "task.h"


namespace NRTSimulator {
	void setRealtimePriority(int priority) {
		struct sched_param param;
		param.sched_priority = priority;
		sched_setscheduler(0, SCHED_FIFO, &param);
	}
}


int main(int argc, char *argv[])
{
	NRTSimulator::setRealtimePriority(80);

	long long executionTime (20 * 1000 * 1000);
	long long period (50 * 1000 * 1000);

	long long offset = std::chrono::duration_cast<std::chrono::nanoseconds>(
			(std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();

	long long endSimulation = std::chrono::duration_cast<std::chrono::nanoseconds>(
			(std::chrono::high_resolution_clock::now() + std::chrono::seconds(7)).time_since_epoch()).count();

	NRTSimulator::TTask task(executionTime, period, 5.5);	

	std::cout << "Worst case execution time: " << task.Run(offset, endSimulation) << std::endl;
}
