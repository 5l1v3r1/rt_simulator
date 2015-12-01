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
	void setRealtimePriority(int priority, const std::string & taskName) {
		struct sched_param param;
		param.sched_priority = priority;
		if (sched_setscheduler(0, SCHED_FIFO, &param)) {
			std::cerr << taskName << ": failed to set RT priority." << std::endl;
			exit(-1);  
		}
	}

	void setAffinity(int cpu, const std::string & taskName) {
		cpu_set_t set;
		CPU_ZERO(&set);
        CPU_SET(cpu, &set);

        if (sched_setaffinity(0, sizeof(set), &set) == -1) {
        	std::cerr << taskName << ": failed to set cpu." << std::endl;
        	exit(-1);
        }            
	}
}


int main(int argc, char *argv[]) {	
	NRTSimulator::TTaskArgParser parser;
	parser.Parse(argc, argv);

	std::unique_ptr<NRTSimulator::TTask> task(new NRTSimulator::TTimerTask(parser.GetExecutionTime(), parser.GetPeriod()));

	NRTSimulator::setRealtimePriority(parser.GetPriority(), parser.GetTaskName());
	NRTSimulator::setAffinity(parser.GetCPU(), parser.GetTaskName());

	std::cout << parser.GetTaskName() << ": worst case responce time " <<
				 task->Run(parser.GetOffset(), parser.GetEnd()).count() << std::endl;
	return 0;
}
