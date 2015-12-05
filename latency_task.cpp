#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>

#include "latency_task.h"

namespace NRTSimulator
{
	static void * runLatencyTask(void * params)
	{
		TLatencyTaskParams * taskParams = (TLatencyTaskParams*)params;

		cpu_set_t set;
		CPU_ZERO(&set);
		CPU_SET(taskParams->CPU, &set);

		if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == -1) {
		    std::cerr << "Failed to set cpu in latency task." << std::endl;
		    return NULL;
		}
		while(std::chrono::high_resolution_clock::now() < taskParams->End) {
		    int p = fork();
		    if (p == 0) {
		        sleep(100);
		    } else {
		        kill(p, SIGTERM);
		    }
		    usleep(1000);
		}

		return NULL;
	}

	TLatencyTaskSet::TLatencyTaskSet(const std::vector <int> cpus)
		: CPUS(cpus)
		, Params(cpus.size())
		, ThreadIds(cpus.size())		
	{

	}
	void TLatencyTaskSet::Run(
	    std::chrono::time_point<std::chrono::high_resolution_clock> start,
	    std::chrono::time_point<std::chrono::high_resolution_clock> end)
	{
		for (size_t i = 0; i < CPUS.size(); ++i) {
			Params[i].CPU = CPUS[i];
			Params[i].Start = start;
			Params[i].End = end;
			pthread_create(&ThreadIds[i], NULL, &runLatencyTask, &Params[i]);
		}
	}

	void TLatencyTaskSet::Join() {
		for (size_t i = 0; i < CPUS.size(); ++i) {
			void ** dummy = NULL;
			pthread_join(ThreadIds[i], dummy);
		}
	}



}
