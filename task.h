#pragma once

#include <sys/time.h>
#include <signal.h>
#include <time.h>


namespace NRTSimulator {
	class TTask
	{
	private:
		long long ExecutionTime;
		long long Period;
		long long Offset;		
		long long CountTo;

		timer_t JobFireTimer;
		sigset_t AlarmSignal;
		struct itimerspec JobDoneTimeSpec;
	public:
		//ConvertRate how many ns one "long long" addition take.
		TTask(long long executionTime, long long  period, long long offset, double convertRate);
		void Run();
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