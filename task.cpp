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

    const long long NUMBER_OF_NANOSECONDS_IN_SECOND = 
            std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::seconds(1)).count();

    TTask::TTask(const TRandomVar & executionTime, long long period)
        : ExecutionTime(executionTime)
        , Period(period)
    {
    }

    TTask::~TTask() {
        timer_delete(JobFireTimer);
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
        sigprocmask(SIG_BLOCK, &AlarmSignal, NULL);
    }

    void TTask::InitializeFireTimerSpec() {
        long long offsetNanosecond = std::chrono::nanoseconds(StartSimulation.time_since_epoch()).count();
        JobFireTimeSpec.it_value.tv_sec = offsetNanosecond / NUMBER_OF_NANOSECONDS_IN_SECOND;
        JobFireTimeSpec.it_value.tv_nsec = offsetNanosecond % NUMBER_OF_NANOSECONDS_IN_SECOND;
        JobFireTimeSpec.it_interval.tv_sec = Period.count() / NUMBER_OF_NANOSECONDS_IN_SECOND;
        JobFireTimeSpec.it_interval.tv_nsec = Period.count() % NUMBER_OF_NANOSECONDS_IN_SECOND;
    }

    void TTask::Initialize() {
        InitializeFireTimer();
        InitializeFireAlarmSignal();
        InitializeFireTimerSpec();
    }

    std::chrono::nanoseconds TTask::Run(long long startAt, long long endAt) {
        StartSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(startAt));
        EndSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(endAt));
        Initialize();
        TaskBody();        
        timer_delete(JobFireTimer);
        return WorstCaseResponce;
    }

    void TTask::TaskBody() {
        timer_settime(JobFireTimer, TIMER_ABSTIME, &JobFireTimeSpec, NULL);

        WorstCaseResponce = std::chrono::nanoseconds(0);

        std::chrono::time_point<std::chrono::high_resolution_clock> taskFired (StartSimulation);
        while (true) {
            WaitForNextActivation();             
            long long executionTime = ExecutionTime.Sample();                      
            JobBody(executionTime);
            auto taskProcessed = std::chrono::high_resolution_clock::now();    
            auto responceTime = std::chrono::duration_cast<std::chrono::nanoseconds>(taskProcessed - taskFired);
            //std::cout << responceTime << std::endl;

            WorstCaseResponce = std::max(responceTime, WorstCaseResponce);
            
            if (taskProcessed > EndSimulation) {
                return;
            }
            taskFired += Period;
        }
    }

    void TTask::WaitForNextActivation() {
        int dummy;
        sigwait(&AlarmSignal, &dummy);
    }


    TCountingTask::TCountingTask(const TRandomVar & executionTime, long long period) 
        : TTask(executionTime, period)
    {}

    void TCountingTask::JobBody(long long executionTime) {
        long long countTo = executionTime / ConvertRate;
        for (long long i = 0; i < countTo; ++i) {
              //Just counting
        }
    }

    TCountingTask::~TCountingTask() {}

    TTimerTask::TTimerTask(const TRandomVar & executionTime, long long period)
        : TTask(executionTime, period)
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
        jobDoneEvent.sigev_signo = SIGALRM;

        if (timer_create(CLOCK_PROCESS_CPUTIME_ID, &jobDoneEvent, &JobDoneTimer) == -1) {
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