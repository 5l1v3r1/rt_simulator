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
        long long nsInS = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::seconds(1)).count();
        JobFireTimeSpec.it_value.tv_sec = Offset / nsInS;
        JobFireTimeSpec.it_value.tv_nsec = Offset % nsInS;
        JobFireTimeSpec.it_interval.tv_sec = Period / nsInS;
        JobFireTimeSpec.it_interval.tv_nsec = Period % nsInS;
    }

    void TTask::Initialize() {
        InitializeFireTimer();
        InitializeFireAlarmSignal();
        InitializeFireTimerSpec();
    }

    long long TTask::Run(long long startAt, long long endAt) {
        Offset = startAt;
        EndSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(endAt));
        Initialize();
        TaskBody();        
        timer_delete(JobFireTimer);
        return WorstCaseResponce;
    }

    void TTask::TaskBody() {
        timer_settime(JobFireTimer, TIMER_ABSTIME, &JobFireTimeSpec, NULL);

        WorstCaseResponce = -1;

        while (true) {
            WaitForNextActivation();
            auto start = std::chrono::high_resolution_clock::now();  
            long long executionTime = ExecutionTime.Sample();                      
            JobBody(executionTime);
            auto end = std::chrono::high_resolution_clock::now();      
            long long responceTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            //std::cout << responceTime << std::endl;

            WorstCaseResponce = std::max(responceTime, WorstCaseResponce);
            
            if (end > EndSimulation) {
                return;
            }

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
        JobDoneTimeSpec.it_value.tv_sec = executionTime / 1000000000;
        JobDoneTimeSpec.it_value.tv_nsec = executionTime % 1000000000;

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