#include <algorithm>
#include <limits>
#include <iostream>

#include "rta.h"

namespace NRTSimulator {
	TRTA::TRTA(const std::vector<std::shared_ptr<TTask>> & tasks)
		: WorstCaseResponceTime(tasks.size(), -1)
		, IsSchedulable(tasks.size(), true)
		, WorstCaseExecutionTime(tasks.size())
		, Period(tasks.size())
		, CPU(tasks.size())
		, Priority(tasks.size())
	{
		std::transform(tasks.begin(), tasks.end(), WorstCaseExecutionTime.begin(),
				[] (const std::shared_ptr<TTask> & task) {return task->GetWorstCaseExecutionTime();});
		std::transform(tasks.begin(), tasks.end(), CPU.begin(), 
				[] (const std::shared_ptr<TTask> & task) {return task->GetCpu();});
		std::transform(tasks.begin(), tasks.end(), Period.begin(),
				[] (const std::shared_ptr<TTask> & task) {return task->GetPeriod();});
		std::transform(tasks.begin(), tasks.end(), Priority.begin(), 
				[] (const std::shared_ptr<TTask> & task) {return task->GetPriority();});
	}
	void TRTA::Compute() {
		for (size_t taskNumber = 0; taskNumber < WorstCaseExecutionTime.size(); ++taskNumber) {
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
		long long nextResponceTime = WorstCaseExecutionTime[taskNumber];

		while (true) {
			previousResponceTime = nextResponceTime;
			nextResponceTime = WorstCaseExecutionTime[taskNumber];
			for (size_t i = 0; i < WorstCaseExecutionTime.size(); ++i) {
				if (CPU[i] == CPU[taskNumber] && Priority[i] > Priority[taskNumber]) {
					Compute(i);
					if (!IsSchedulable[i]) {
						IsSchedulable[taskNumber] = false;
						WorstCaseResponceTime[taskNumber] = std::numeric_limits<long long>::max();
						return;
					}
					nextResponceTime += 
						std::ceil(((long double)previousResponceTime) / Period[i]) * WorstCaseExecutionTime[i];
				}
			}
			if (nextResponceTime == previousResponceTime) {
				IsSchedulable[taskNumber] = true;
				WorstCaseResponceTime[taskNumber] = nextResponceTime;
				return;
			}
			if (nextResponceTime > Period[taskNumber]) {
				IsSchedulable[taskNumber] = false;
				WorstCaseResponceTime[taskNumber] = std::numeric_limits<long long>::max();
				return;
			}
		}
	}
}