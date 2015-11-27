#pragma once

#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <chrono>


namespace NRTSimulator {
	class TTask
	{
	private:
		long long ExecutionTime;
		long long Period;
			
		long long CountTo;

		long long Offset;
		std::chrono::time_point<std::chrono::high_resolution_clock> EndSimulation;

		long long WorstCaseExecution;

		timer_t JobFireTimer;
		sigset_t AlarmSignal;
		struct itimerspec JobFireTimeSpec;
	public:
		//ConvertRate how many ns one "long long" addition take.
		TTask(long long executionTime, long long  period,  double convertRate);
		long long Run(long long startAt, long long endAt);
		~TTask();
	private:
		void InitializeTimer();
		void InitializeAlarmSignal();

		void InitializeTimerSpec();
		void TaskBody();

		void WaitForNextActivation();
		void JobBody();	
	};

}