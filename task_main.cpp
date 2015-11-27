#include <unistd.h>
#include <chrono>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>

#include <map>

#include "task.h"
#include "random_var.h"


namespace NRTSimulator {
	void setRealtimePriority(int priority) {
		struct sched_param param;
		param.sched_priority = priority;
		sched_setscheduler(0, SCHED_FIFO, &param);
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

	TTask parseTaskParameters(const int argc, const char * const argv[], long long & start, long long & end, int & priority) {

		// Usage: Priority CountExecutions Mass_i Execution_i Period Offset End ConvertRate
		if (argc < 7) {
			std::cout << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}

		int currentArgIndex = 0;
		priority = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		int countExecutions = stringToLong(argv[currentArgIndex]);
		++currentArgIndex;

		if ( 2 * countExecutions != argc - 6) {
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

		double convertRate = stringToDouble(argv[currentArgIndex]);		

		NRTSimulator::TTask task(executionTime, period, convertRate);

		return task;		
	}
}


int main(int argc, char *argv[])
{
	//TODO: set process affinity
	//TODO: remove this part
	const int arg_count = 14;
	char * args[arg_count];
	for (int i = 0; i < arg_count; ++i) {
		args[i] = new char[255];
	}
	args[0] = "80";
	args[1] = "4";
	args[2] = "0.1";
	args[3] = "10000000";
	args[4] = "0.4";
	args[5] = "20000000";
	args[6] = "0.2";
	args[7] = "30000000";
	args[8] = "0.3";
	args[9] = "30000000";
	args[10] = "100000000";
	std::stringstream s;
	s << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();
	strcpy(args[11], s.str().c_str());
	std::stringstream e;
	e << std::chrono::duration_cast<std::chrono::nanoseconds>
					((std::chrono::high_resolution_clock::now() + std::chrono::seconds(7)).time_since_epoch()).count();
	strcpy(args[12], e.str().c_str());
	args[13] = "4.1";
	//////////////////////////////



	int priority;
	long long offset, endSimulation;

	NRTSimulator::TTask task = NRTSimulator::parseTaskParameters(arg_count, args, offset, endSimulation, priority);

	NRTSimulator::setRealtimePriority(priority);

	std::cout << "Worst case execution time: " << task.Run(offset, endSimulation) << std::endl;
}
