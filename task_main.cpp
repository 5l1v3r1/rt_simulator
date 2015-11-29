#include <unistd.h>
#include <chrono>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

#include <memory>

#include "task.h"
#include "random_var.h"
#include "task_arg_parser.h"


namespace NRTSimulator {
	void setRealtimePriority(int priority) {
		struct sched_param param;
		param.sched_priority = priority;
		if (sched_setscheduler(0, SCHED_FIFO, &param)) {
			std::cout << "Failed to set RT priority." << std::endl;
		}
	}

	void setAffinity(int cpu) {
		cpu_set_t set;
		CPU_ZERO(&set);
        CPU_SET(cpu, &set);

        if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        	std::cout << "Failed to set cpu." << std::endl;
        }            
	}
}


int main(int argc, char *argv[])
{
	//TODO: set process affinity
	//TODO: remove this part
	const int arg_count = 16;
	char * args[arg_count];
	for (int i = 0; i < arg_count; ++i) {
		args[i] = new char[255];
	}
	strcpy(args[0], "task");
	strcpy(args[1], "0");
	strcpy(args[2], "80");
	strcpy(args[3], "100000000");
	std::stringstream s;
	s << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();
	strcpy(args[4], s.str().c_str());
	std::stringstream e;
	e << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(7)).time_since_epoch()).count();
	strcpy(args[5], e.str().c_str());

	strcpy(args[6], "4");
	strcpy(args[7], "0.1");
	strcpy(args[8], "10000000");
	strcpy(args[9], "0.4");
	strcpy(args[10], "20000000");
	strcpy(args[11], "0.2");
	strcpy(args[12], "30000000");
	strcpy(args[13], "0.3");
	strcpy(args[14], "40000000");

	//////////////////////////////


	NRTSimulator::TTaskArgParser parser;
	parser.Parse(arg_count, args);

	std::unique_ptr<NRTSimulator::TTask> task(new NRTSimulator::TTimerTask(parser.GetExecutionTime(), parser.GetPeriod()));

	NRTSimulator::setRealtimePriority(parser.GetPriority());
	NRTSimulator::setAffinity(parser.GetCPU());

	std::cout << "Worst case execution time: " << task->Run(parser.GetOffset(), parser.GetEnd()) << std::endl;
}
