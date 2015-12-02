#pragma once
#include <vector>

#include "tasks_file_parser.h"

namespace NRTSimulator {
	class TRTA {
	private:
		std::vector<long long> WorstCaseResponceTime;
		std::vector<bool> IsSchedulable;
		std::vector<long long> WorstCaseExecutionTime;
		std::vector<long long> Period;
		std::vector<int> CPU;
		std::vector<int> Priority;		
	public:
		TRTA(const std::vector<std::shared_ptr<TTask>> &);
		void Compute();
		long long GetWorstCaseResponceTime(size_t taskNumber);
		bool CheckIsShedulable(size_t taskNumber);
	private:
		void Compute(size_t taskNumber);
	};
}