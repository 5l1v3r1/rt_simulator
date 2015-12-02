#pragma once
#include <vector>

#include "tasks_file_parser.h"

namespace NRTSimulator {
	class TRTA {
	private:
		std::vector<long long> WorstCaseResponceTime;
		std::vector<bool> IsSchedulable;
		const std::vector<std::shared_ptr<TTask>> & Tasks;
	public:
		TRTA(const std::vector<std::shared_ptr<TTask>> &);
		void Compute();
		long long GetWorstCaseResponceTime(size_t taskNumber);
		bool CheckIsShedulable(size_t taskNumber);
	private:
		void Compute(size_t taskNumber);
	};
}