#include <iostream>
#include <sstream>
#include "task_arg_parser.h"

namespace NRTSimulator {

	const std::string FAILED_TO_PARSE_ARG_MESSAGE("Failed to parse comand line options. Don`t run task file directly!!!");

	long long TTaskArgParser::stringToLong(const std::string s) {
		std::stringstream stream(s);
		long long val;
		stream >> val;
		if (!stream) {
			std::cerr << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}
		return val;
	}
	double TTaskArgParser::stringToDouble(const std::string s) {
		std::stringstream stream(s);
		double val;
		stream >> val;
		if (!stream) {
			std::cerr << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		}
		return val;
	}

	void TTaskArgParser::Parse(int argc, const char * const argv[]) {
		for (int i = 0; i < argc; ++ i) {
			args.push_back(argv[i]);
		}
		if (argc < 8) {
			std::cerr << FAILED_TO_PARSE_ARG_MESSAGE << std::endl;
			exit(-1);
		} 
	}

	std::string TTaskArgParser::GetTaskName() {
		return args[1];
	}

	int TTaskArgParser::GetCPU() {
		return stringToLong(args[2]);
	}
	int TTaskArgParser::GetPriority() {
		return stringToLong(args[3]);
	}
	TRandomVar TTaskArgParser::GetExecutionTime() {
		int countExecutions = stringToLong(args[7]);

		std::vector<double> mass(countExecutions);
		std::vector<long long> values(countExecutions);
		for (int i = 0; i < countExecutions; ++ i) {
			mass[i] = stringToDouble(args[2 * i + 8]);
			values[i] = stringToLong(args[2 * i + 9]);
		}
		return TRandomVar(mass, values);
	}
	long long TTaskArgParser::GetPeriod() {
		return stringToLong(args[4]);
	}
	long long TTaskArgParser::GetOffset() {
		return stringToLong(args[5]);
	}
	long long TTaskArgParser::GetEnd() {
		return stringToLong(args[6]);
	}
}