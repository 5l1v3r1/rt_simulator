#include <algorithm>
#include <limits>
#include <iostream>

#include "rta.h"

namespace NRTSimulator {
	TRTA::TRTA(const std::vector<TTaskSpec> & taskSpecs)
		: WorstCaseResponceTime(taskSpecs.size(), -1)
		, IsSchedulable(taskSpecs.size(), true)
		, WorstCaseExecutionTime(taskSpecs.size())
		, Period(taskSpecs.size())
		, CPU(taskSpecs.size())
		, Priority(taskSpecs.size())
	{
		for (size_t i = 0; i < WorstCaseResponceTime.size(); ++i) {
			WorstCaseExecutionTime[i] = std::accumulate(taskSpecs[i].ExecutionTimeSpec.begin(),
			 	taskSpecs[i].ExecutionTimeSpec.end(), 0, [](long long accumulator, std::pair<double, long long> pair) {
					return std::max(accumulator, pair.second);
				});
		}
		std::transform(taskSpecs.begin(), taskSpecs.end(), CPU.begin(), 
				[] (const TTaskSpec & taskSpec) {return taskSpec.CPU;});
		std::transform(taskSpecs.begin(), taskSpecs.end(), Period.begin(),
				[] (const TTaskSpec & taskSpec) {return taskSpec.Period;});
		std::transform(taskSpecs.begin(), taskSpecs.end(), Priority.begin(), 
				[] (const TTaskSpec & taskSpec) {return taskSpec.Priority;});
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