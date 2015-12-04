#include <algorithm>
#include <limits>
#include <iostream>

#include "rta.h"

namespace NRTSimulator {
	TRTA::TRTA(const std::vector<std::shared_ptr<TTask>> & tasks)
		: WorstCaseResponceTime(tasks.size(), -1)
		, IsSchedulable(tasks.size(), true)
		, Tasks(tasks)
		, WorstCaseKernelLatency(0)
	{
	}
	void TRTA::Compute() {
		IsSchedulable = std::vector<bool>(Tasks.size(), true);
		WorstCaseResponceTime = std::vector<long long>(Tasks.size(), -1);
		for (size_t taskNumber = 0; taskNumber < Tasks.size(); ++taskNumber) {
			Compute(taskNumber);
		}
	}
	long long TRTA::GetWorstCaseResponceTime(size_t taskNumber) {
		return WorstCaseResponceTime[taskNumber];
	}
	bool TRTA::CheckIsShedulable(size_t taskNumber) {
		return IsSchedulable[taskNumber];
	}

	long long TRTA::EstimateWorstCaseKernellLatency() {
		for (const auto & task: Tasks) {
			if (task->GetWorstCaseResponceTime() > task->GetPeriod()) {
				return -1;
			}
		}
		WorstCaseKernelLatency = 0;
		long long previousLatency = -1;
		while(true) {
			IsSchedulable = std::vector<bool>(Tasks.size(), true);
			WorstCaseResponceTime = std::vector<long long>(Tasks.size(), -1);
			Compute();
			bool stop = true;
			for (size_t i = 0; i < Tasks.size(); ++i) {
				if (!IsSchedulable[i]) {
					stop = true;
					break;
				}
				if (WorstCaseResponceTime[i] < Tasks[i]->GetWorstCaseResponceTime()) {
					stop = false;
					break;
				}
			}
			if (stop) {
				WorstCaseKernelLatency = 0;
				return previousLatency;
			}
			previousLatency = WorstCaseKernelLatency;
			WorstCaseKernelLatency += WorstCaseKernelLatencyStep;
		}
	}

	void TRTA::Compute(size_t taskNumber) {
		if (WorstCaseResponceTime[taskNumber] != -1) {
			return;
		}
		long long previousResponceTime = 0;
		long long nextResponceTime = Tasks[taskNumber]->GetWorstCaseExecutionTime() + WorstCaseKernelLatency;

		while (true) {
			previousResponceTime = nextResponceTime;
			nextResponceTime = Tasks[taskNumber]->GetWorstCaseExecutionTime() + WorstCaseKernelLatency;
			for (size_t i = 0; i < Tasks.size(); ++i) {
				if (Tasks[i]->GetCpu() == Tasks[taskNumber]->GetCpu() 
					&& Tasks[i]->GetPriority() > Tasks[taskNumber]->GetPriority()) {
					Compute(i);
					if (!IsSchedulable[i]) {
						IsSchedulable[taskNumber] = false;
						WorstCaseResponceTime[taskNumber] = std::numeric_limits<long long>::max();
						return;
					}
					nextResponceTime +=	std::ceil(((long double)previousResponceTime) / Tasks[i]->GetPeriod()) * 
								Tasks[i]->GetWorstCaseExecutionTime();
				}
			}
			if (nextResponceTime == previousResponceTime) {
				IsSchedulable[taskNumber] = true;
				WorstCaseResponceTime[taskNumber] = nextResponceTime;
				return;
			}
			if (nextResponceTime > Tasks[taskNumber]->GetPeriod()) {
				IsSchedulable[taskNumber] = false;
				WorstCaseResponceTime[taskNumber] = std::numeric_limits<long long>::max();
				return;
			}
		}
	}
}