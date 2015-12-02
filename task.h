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
		std::chrono::nanoseconds Period;

		std::chrono::time_point<std::chrono::high_resolution_clock> EndSimulation;
		std::chrono::time_point<std::chrono::high_resolution_clock> NextTaskFire;

		std::chrono::nanoseconds WorstCaseResponce;

		timer_t JobFireTimer;
		sigset_t AlarmSignal;
		struct timespec JobFireTimeSpec;

		int CPU;
		int Priority;
		std::string Name;		
	public:		
		TTask(const TRandomVar & executionTime, long long period, int cpu, int priority, const std::string & name);
		void Run(long long startAt, long long endAt);

		int GetCpu() const;
		int GetPriority() const;
		long long GetWorstCaseExecutionTime() const;
		const std::string & GetName() const;
		long long GetPeriod() const;

		long long GetWorstCaseResponceTime() const;		

		virtual ~TTask();
	protected:
		virtual void Initialize();
	private:
		void InitializeFireTimer();
		void InitializeFireAlarmSignal();
		void ComputeFireTimerSpec();

		void TaskBody();

		void SetUpCPU();

		void SetUpPriority();

		void WaitForNextActivation();
		
		virtual void JobBody(long long executionTime) = 0;	
	};

	class TCountingTask : public TTask
	{
	private:
		const double ConvertRate = 6; //ConvertRate how many ns one "long long" addition take.
	public:
		TCountingTask(const TRandomVar & executionTime, long long period, int cpu, int priority, const std::string & name);
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
		TTimerTask(const TRandomVar & executionTime, long long period, int cpu, int priority, const std::string & name);
		virtual ~TTimerTask();
	protected:
		virtual void Initialize() override;
	private:
		virtual void JobBody(long long) override;
		
		void InitializeDoneTimer();
		void InitializeDoneTimerSpec();
	};

}