#pragma once
#include <string>
#include <vector>

#include "random_var.h"


namespace NRTSimulator {
	class TTaskArgParser {
	private:
		std::vector<std::string> args;
	public:
		// TaskName CPU Priority Period Offset End CountExecutions Mass_i Execution_i
		void Parse(int argc, const char * const argv[]);
		int GetCPU();
		int GetPriority();
		TRandomVar GetExecutionTime();
		long long GetPeriod();
		long long GetOffset();
		long long GetEnd();
		const std::string & GetTaskName();
	};
}