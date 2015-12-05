#include <chrono>
#include <iostream>
#include <algorithm>
#include <pthread.h>

#include "task.h"

namespace NRTSimulator
{

	static const long long NUMBER_OF_NANOSECONDS_IN_SECOND =
	    std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::seconds(
	                1)).count();

	void * runTask(void * params)
	{
		TTask * task = (TTask*)params;
		task->TaskBody();
		return NULL;
	}

	TTask::TTask(const TRandomVar & executionTime, long long period,
	             int cpu, int priority, const std::string & name)
		: ExecutionTime(executionTime)
		, Period(period)
		, CPU(cpu)
		, Priority(priority)
		, Name(name)
	{
	}

	TTask::~TTask()
	{
	}

	void TTask::SetUpPriority()
	{
		struct sched_param param;
		param.sched_priority = Priority;

		if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param)) {
			std::cerr << Name << ": failed to set RT priority." << std::endl;
			exit(-1);
		}
	}

	void TTask::SetUpCPU()
	{
		cpu_set_t set;
		CPU_ZERO(&set);
		CPU_SET(CPU, &set);

		if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == -1) {
			std::cerr << Name << ": failed to set cpu." << std::endl;
			exit(-1);
		}
	}

	long long TTask::GetPeriod() const
	{
		return Period.count();
	}

	int TTask::GetCpu() const
	{
		return CPU;
	}
	int TTask::GetPriority() const
	{
		return Priority;
	}
	long long TTask::GetWorstCaseExecutionTime() const
	{
		return ExecutionTime.GetMaxValue();
	}
	const std::string & TTask::GetName() const
	{
		return Name;
	}
	long long TTask::GetWorstCaseResponceTime() const
	{
		return (*std::max_element(ResponceTimes.begin(), ResponceTimes.end())).count();
	}

	std::vector<long long> TTask::GetResponceTimes() const
	{
		std::vector<long long> res(ResponceTimes.size());
		std::transform(ResponceTimes.begin(), ResponceTimes.end(), res.begin(),
		[](const std::chrono::nanoseconds & duration) {return duration.count();});
		return res;
	}

	void TTask::ComputeFireTimerSpec()
	{
		long long offsetNanosecond = std::chrono::nanoseconds(
		                                 NextTaskFire.time_since_epoch()).count();
		JobFireTimeSpec.tv_sec = offsetNanosecond / NUMBER_OF_NANOSECONDS_IN_SECOND;
		JobFireTimeSpec.tv_nsec = offsetNanosecond % NUMBER_OF_NANOSECONDS_IN_SECOND;
	}

	void TTask::Initialize()
	{
		ResponceTimes.reserve((EndSimulation - NextTaskFire) / Period + 1);
		SetUpPriority();
		SetUpCPU();
	}

	void TTask::Run(long long startAt, long long endAt)
	{
		NextTaskFire = std::chrono::time_point<std::chrono::high_resolution_clock>
		               (std::chrono::nanoseconds(startAt));
		EndSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>
		                (std::chrono::nanoseconds(endAt));
		pthread_create(&ThreadId, NULL, &runTask, this);
	}

	void TTask::Join()
	{
		void ** dummy = NULL;
		pthread_join(ThreadId, dummy);
	}

	void TTask::TaskBody()
	{
		Initialize();

		while (NextTaskFire < EndSimulation) {
			WaitForNextActivation();
			long long executionTime = ExecutionTime.Sample();
			JobBody(executionTime);
			auto taskProcessed = std::chrono::high_resolution_clock::now();
			auto responceTime = std::chrono::duration_cast<std::chrono::nanoseconds>
			                    (taskProcessed - NextTaskFire);
			//std::cout << responceTime.count() << std::endl;

			ResponceTimes.push_back(responceTime);

			NextTaskFire += Period;
		}
	}

	void TTask::WaitForNextActivation()
	{
		ComputeFireTimerSpec();
		clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &JobFireTimeSpec, NULL);
	}


	TTimerTask::TTimerTask(const TRandomVar & executionTime, long long period,
	                       int cpu, int priority, const std::string & name)
		: TTask(executionTime, period, cpu, priority, name)
	{
	}

	void TTimerTask::JobBody(long long executionTime)
	{
		clock_gettime(CLOCK_THREAD_CPUTIME_ID, &JobStartCPUClockTimeSpec);
		long long start_time = JobStartCPUClockTimeSpec.tv_sec *
		                       NUMBER_OF_NANOSECONDS_IN_SECOND +
		                       JobStartCPUClockTimeSpec.tv_nsec;

		while (true) {
			clock_gettime(CLOCK_THREAD_CPUTIME_ID, &JobDoneTimeSpec);
			long long current_time =
			    JobDoneTimeSpec.tv_sec * NUMBER_OF_NANOSECONDS_IN_SECOND +
			    JobDoneTimeSpec.tv_nsec;

			if (start_time + executionTime < current_time) {
				return;
			}
		}
	}

	TTimerTask::~TTimerTask() {}


	TCountingTask::TCountingTask(const TRandomVar & executionTime, long long period,
	                             int cpu, int priority, const std::string & name)
		: TTask(executionTime, period, cpu, priority, name)
	{}

	void TCountingTask::JobBody(long long executionTime)
	{
		long long countTo = executionTime / ConvertRate;

		for (long long i = 0; i < countTo; ++i) {
			//Just counting
		}
	}

	TCountingTask::~TCountingTask() {}


	double TCountingTask::ConvertRate;
	void TCountingTask::EstimateConvertRate()
	{
		struct sched_param previousParam;
		int previousPolicy;
		pthread_getschedparam(pthread_self(), &previousPolicy, &previousParam);

		struct sched_param param;
		param.sched_priority = 80;

		if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param)) {
			std::cerr << "Main: failed to set RT priority." << std::endl;
			exit(-1);
		}

		auto start = std::chrono::high_resolution_clock::now();
		long long countTo = 1000000000;

		for (long long i = 0; i < countTo; ++i) {

		}

		long long countDuration = std::chrono::duration_cast<std::chrono::nanoseconds>
		                          (std::chrono::high_resolution_clock::now() - start).count();

		if (pthread_setschedparam(pthread_self(), previousPolicy, &previousParam)) {
			std::cerr << "Main: failed to return to normal priority." << std::endl;
			exit(-1);
		}

		ConvertRate = countDuration / (long double) countTo;
	}
}