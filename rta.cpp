#include <algorithm>
#include <limits>
#include <iostream>

#include "rta.h"

namespace NRTSimulator {
	TRTA::TRTA(const std::vector<std::shared_ptr<TTask>> & tasks)
		: WorstCaseResponceTime(tasks.size(), -1)
		, IsSchedulable(tasks.size(), true)
		, Tasks(tasks)
	{
	}
	void TRTA::Compute() {
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

	void TRTA::Compute(size_t taskNumber) {
		if (WorstCaseResponceTime[taskNumber] != -1) {
			return;
		}
		long long previousResponceTime = 0;
		long long nextResponceTime = Tasks[taskNumber]->GetWorstCaseExecutionTime();

		while (true) {
			previousResponceTime = nextResponceTime;
			nextResponceTime = Tasks[taskNumber]->GetWorstCaseExecutionTime();
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