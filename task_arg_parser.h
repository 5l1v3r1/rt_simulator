#pragma once
#include <string>
#include <vector>

#include "random_var.h"


namespace NRTSimulator {
	class TTaskArgParser {
	private:
		std::vector<std::string> args;
	public:
		// CPU Priority Period Offset End CountExecutions Mass_i Execution_i
		void Parse(int argc, char *argv[]);
		int GetCPU();
		int GetPriority();
		TRandomVar GetExecutionTime();
		long long GetPeriod();
		long long GetOffset();
		long long GetEnd();
	private:
		long long stringToLong(const std::string str);
		double stringToDouble(const std::string str);
	};
}