#pragma once
#include <vector>

#include "tasks_file_parser.h"

namespace NRTSimulator {
	class TRTA {
	private:
		std::vector<long long> WorstCaseResponceTime;
		std::vector<bool> IsSchedulable;
		const std::vector<std::shared_ptr<TTask>> & Tasks;
		long long WorstCaseKernelLatency;
		const long long WorstCaseKernelLatencyStep = 1000;
	public:
		TRTA(const std::vector<std::shared_ptr<TTask>> &);
		void Compute();
		long long GetWorstCaseResponceTime(size_t taskNumber);
		bool CheckIsShedulable(size_t taskNumber);
		long long EstimateWorstCaseKernellLatency();
	private:
		void Compute(size_t taskNumber);
	};
}