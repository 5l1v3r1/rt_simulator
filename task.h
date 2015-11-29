#pragma once

#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <chrono>

#include "random_var.h"

namespace NRTSimulator {
	class TTask
	{
	private:
		TRandomVar ExecutionTime;
		long long Period;

		long long Offset;
		std::chrono::time_point<std::chrono::high_resolution_clock> EndSimulation;

		long long WorstCaseResponce;

		timer_t JobFireTimer;
		sigset_t AlarmSignal;
		struct itimerspec JobFireTimeSpec;
		
	public:		
		TTask(const TRandomVar & executionTime, long long  period);
		long long Run(long long startAt, long long endAt);
		virtual ~TTask();
	protected:
		virtual void Initialize();
	private:
		void InitializeFireTimer();
		void InitializeFireAlarmSignal();
		void InitializeFireTimerSpec();

		void TaskBody();

		void WaitForNextActivation();
		
		virtual void JobBody(long long executionTime) = 0;	
	};

	class TCountingTask : public TTask
	{
	private:
		const double ConvertRate = 6; //ConvertRate how many ns one "long long" addition take.
	public:
		TCountingTask(const TRandomVar & executionTime, long long  period);
		virtual ~TCountingTask();
	private:
		virtual void JobBody(long long) override;
	};

	class TTimerTask : public TTask
	{
	private:
		timer_t JobDoneTimer;
		struct itimerspec JobDoneTimeSpec;
	public:
		TTimerTask(const TRandomVar & executionTime, long long  period);
		virtual ~TTimerTask();
	protected:
		virtual void Initialize() override;
	private:
		virtual void JobBody(long long) override;
		
		void InitializeDoneTimer();
		void InitializeDoneTimerSpec();
	};

}