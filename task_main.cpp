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


	const std::string FAILED_TO_PARSE_ARG_MESSAGE("Failed to parse comand line options. Don`t run task file directly!!!");
	long long stringToLong(const char * const  string) {
		std::stringstream str(string);
		long long val;
		str >> val;
		if (!str) {
			std::cout << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}
		return val;
	}
	double stringToDouble(const char * const  string) {
		std::stringstream str(string);
		double val;
		str >> val;
		if (!str) {
			std::cout << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}
		return val;
	}

	std::shared_ptr<TTask> parseTaskParameters(const int argc, const char * const argv[], long long & start, 
			long long & end, int & priority, int & cpu) {

		// Usage: CPU Priority CountExecutions Mass_i Execution_i Period Offset End
		if (argc < 7) {
			std::cout << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}

		int currentArgIndex = 1;

		cpu = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		priority = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		int countExecutions = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		if ( 2 * countExecutions != argc - 8) {
			std::cout << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}

		std::vector<double> mass(countExecutions);
		std::vector<long long> values(countExecutions);
		for (int i = 0; i < countExecutions; ++ i) {
			mass[i] = stringToDouble(argv[currentArgIndex]);
			++currentArgIndex;

			values[i] = stringToLong(argv[currentArgIndex]);
			++currentArgIndex;
		}

		TRandomVar executionTime (mass, values);


		long long period = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		start = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		end = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		//double convertRate = stringToDouble(argv[currentArgIndex]);		

		NRTSimulator::TTask* task = new NRTSimulator::TTimerTask(executionTime, period);

		return std::shared_ptr<TTask>(task);
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
	strcpy(args[3], "4");
	strcpy(args[4], "0.1");
	strcpy(args[5], "20000000");
	strcpy(args[6], "0.4");
	strcpy(args[7], "20000000");
	strcpy(args[8], "0.2");
	strcpy(args[9], "20000000");
	strcpy(args[10], "0.3");
	strcpy(args[11], "20000000");
	strcpy(args[12], "100000000");
	std::stringstream s;
	s << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();
	strcpy(args[13], s.str().c_str());
	std::stringstream e;
	e << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(7)).time_since_epoch()).count();
	strcpy(args[14], e.str().c_str());
	//args[15] = "6";
	//////////////////////////////


	int priority, cpu;
	long long offset, endSimulation;

	auto task = NRTSimulator::parseTaskParameters(arg_count, args, offset, endSimulation, priority, cpu);

	NRTSimulator::setRealtimePriority(priority);
	NRTSimulator::setAffinity(cpu);

	std::cout << "Worst case execution time: " << task->Run(offset, endSimulation) << std::endl;
}
