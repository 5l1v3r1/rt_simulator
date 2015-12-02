#include <chrono>
#include <iostream>
#include <cstring>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <limits>

#include "task.h"


namespace NRTSimulator {

    static const long long NUMBER_OF_NANOSECONDS_IN_SECOND = 
            std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::seconds(1)).count();

    TTask::TTask(const TRandomVar & executionTime, long long period,
                 int cpu, int priority, const std::string & name)
        : ExecutionTime(executionTime)
        , Period(period)
        , CPU(cpu)
        , Priority(priority)
        , Name(name)
    {
    }

    TTask::~TTask() {
        timer_delete(JobFireTimer);
    }

    void TTask::SetUpPriority() {
        struct sched_param param;
        param.sched_priority = Priority;
        if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param)) {
            std::cerr << Name << ": failed to set RT priority." << std::endl;
            exit(-1);  
        }
    }

    void TTask::SetUpCPU() {
        cpu_set_t set;
        CPU_ZERO(&set);
        CPU_SET(CPU, &set);

        if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == -1) {
            std::cerr << Name << ": failed to set cpu." << std::endl;
            exit(-1);
        } 
    }

    long long TTask::GetPeriod() const {
        return Period.count();
    }

    int TTask::GetCpu() const {
        return CPU;
    }
    int TTask::GetPriority() const {
        return Priority;
    }
    long long TTask::GetWorstCaseExecutionTime() const {
        return ExecutionTime.GetMaxValue();
    }
    const std::string & TTask::GetName() const {
        return Name;
    }
    long long TTask::GetWorstCaseResponceTime() const {
        return WorstCaseResponce.count();
    }

    void TTask::InitializeFireTimer() {
        struct sigevent jobFireEvent;
        memset(&jobFireEvent, 0, sizeof(struct sigevent));
        jobFireEvent.sigev_notify = SIGEV_SIGNAL;
        jobFireEvent.sigev_signo = SIGALRM;
        timer_create(CLOCK_REALTIME, &jobFireEvent, &JobFireTimer);
    }

    void TTask::InitializeFireAlarmSignal() {
        sigemptyset(&AlarmSignal);
        sigaddset(&AlarmSignal, SIGALRM);
        pthread_sigmask(SIG_BLOCK, &AlarmSignal, NULL);
    }

    void TTask::ComputeFireTimerSpec() {
        long long offsetNanosecond = std::chrono::nanoseconds(NextTaskFire.time_since_epoch()).count();
        JobFireTimeSpec.tv_sec = offsetNanosecond / NUMBER_OF_NANOSECONDS_IN_SECOND;
        JobFireTimeSpec.tv_nsec = offsetNanosecond % NUMBER_OF_NANOSECONDS_IN_SECOND;
    }

    void TTask::Initialize() {
        InitializeFireTimer();
        InitializeFireAlarmSignal();
    }

    void TTask::Run(long long startAt, long long endAt) {
        NextTaskFire = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(startAt));
        EndSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(endAt));
        WorstCaseResponce = std::chrono::nanoseconds(0);
        SetUpPriority();
        SetUpCPU();
        Initialize();
        TaskBody();        
        timer_delete(JobFireTimer);
    }

    void TTask::TaskBody() {         
        while (NextTaskFire < EndSimulation) {
            WaitForNextActivation();             
            long long executionTime = ExecutionTime.Sample();                      
            JobBody(executionTime);
            auto taskProcessed = std::chrono::high_resolution_clock::now();    
            auto responceTime = std::chrono::duration_cast<std::chrono::nanoseconds>(taskProcessed - NextTaskFire);
            //std::cout << responceTime.count() << std::endl;

            WorstCaseResponce = std::max(responceTime, WorstCaseResponce);
            
            NextTaskFire += Period;
        }
    }

    void TTask::WaitForNextActivation() {
        ComputeFireTimerSpec();
        clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &JobFireTimeSpec, NULL);
    }


    TCountingTask::TCountingTask(const TRandomVar & executionTime, long long period, 
                                 int cpu, int priority, const std::string & name)
        : TTask(executionTime, period, cpu, priority, name)
    {}

    void TCountingTask::JobBody(long long executionTime) {
        long long countTo = executionTime / ConvertRate;
        for (long long i = 0; i < countTo; ++i) {
              //Just counting
        }
    }

    TCountingTask::~TCountingTask() {}

    TTimerTask::TTimerTask(const TRandomVar & executionTime, long long period, 
                                 int cpu, int priority, const std::string & name)
        : TTask(executionTime, period, cpu, priority, name)
    {
    }

    void TTimerTask::Initialize() {
        TTask::Initialize();
        InitializeDoneTimer();
        InitializeDoneTimerSpec();
    }

    void TTimerTask::InitializeDoneTimer() {
        struct sigevent jobDoneEvent;
        memset(&jobDoneEvent, 0, sizeof(struct sigevent));
        jobDoneEvent.sigev_notify = SIGEV_NONE;

        if (timer_create(CLOCK_THREAD_CPUTIME_ID, &jobDoneEvent, &JobDoneTimer) == -1) {
            std::cout << "Can`t create timer" << std::endl;
        }
    }

    void TTimerTask::InitializeDoneTimerSpec() {
        JobDoneTimeSpec.it_value.tv_sec = 0;
        JobDoneTimeSpec.it_value.tv_nsec = 0;
        JobDoneTimeSpec.it_interval.tv_sec = 0;
        JobDoneTimeSpec.it_interval.tv_nsec = 0;
    }

    void TTimerTask::JobBody(long long executionTime) {
        JobDoneTimeSpec.it_value.tv_sec = executionTime / NUMBER_OF_NANOSECONDS_IN_SECOND;
        JobDoneTimeSpec.it_value.tv_nsec = executionTime % NUMBER_OF_NANOSECONDS_IN_SECOND;

        timer_settime(JobDoneTimer, 0, &JobDoneTimeSpec, NULL);

        struct itimerspec spec;
        timer_gettime(JobDoneTimer, &spec);

        while (spec.it_value.tv_nsec != 0 || spec.it_value.tv_sec != 0) { 
            timer_gettime(JobDoneTimer, &spec);
        }
    }


    TTimerTask::~TTimerTask() {
        timer_delete(JobDoneTimer);
    }
}