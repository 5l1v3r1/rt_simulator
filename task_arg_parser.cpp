#include <iostream>
#include <sstream>
#include "task_arg_parser.h"

namespace NRTSimulator {

	void TTaskArgParser::Parse(int argc, const char * const argv[]) {
		for (int i = 0; i < argc; ++ i) {
			args.push_back(argv[i]);
		}
		if (argc < 8) {
			std::cerr << ": failed to parse comand line options. Don`t run task file directly!!!" << std::endl;
			exit(-1);
		} 
	}

	const std::string & TTaskArgParser::GetTaskName() {
		return args[1];
	}

	int TTaskArgParser::GetCPU() {
		return stoll(args[2]);
	}
	int TTaskArgParser::GetPriority() {
		return stoll(args[3]);
	}
	TRandomVar TTaskArgParser::GetExecutionTime() {
		int countExecutions = stoll(args[7]);

		std::vector<double> mass(countExecutions);
		std::vector<long long> values(countExecutions);
		for (int i = 0; i < countExecutions; ++ i) {
			mass[i] = stod(args[2 * i + 8]);
			values[i] = stoll(args[2 * i + 9]);
		}
		return TRandomVar(mass, values);
	}
	long long TTaskArgParser::GetPeriod() {
		return stoll(args[4]);
	}
	long long TTaskArgParser::GetOffset() {
		return stoll(args[5]);
	}
	long long TTaskArgParser::GetEnd() {
		return stoll(args[6]);
	}
}