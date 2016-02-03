#include <pthread.h>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <thread>

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

		int p = fork();

		if (p == 0) {
			char * args[1];
			args[0] = new char[1];
			args[0][0] = 0;
			execve(taskParams->HighLatencyScript.c_str(), args, NULL);
		} else {
			std::this_thread::sleep_until(taskParams->End);
			kill(p, SIGTERM);
		}

		return NULL;
	}

	TLatencyTaskSet::TLatencyTaskSet(const std::vector <int> cpus,
	                                 const std::string & highLatencyScript)
		: CPUS(cpus)
		, Params(cpus.size())
		, ThreadIds(cpus.size())
		, HighLatencyScript(highLatencyScript)
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
			Params[i].HighLatencyScript = HighLatencyScript;
			pthread_create(&ThreadIds[i], NULL, &runLatencyTask, &Params[i]);

		}
	}

	void TLatencyTaskSet::Join()
	{
		for (size_t i = 0; i < CPUS.size(); ++i) {
			void ** dummy = NULL;
			pthread_join(ThreadIds[i], dummy);
		}
	}



}
