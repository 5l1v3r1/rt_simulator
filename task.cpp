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


    TTask::TTask(long long executionTime, long long period, double convertRate)
        : ExecutionTime(executionTime)
        , Period(period)
    {
        CountTo = executionTime / convertRate;
    }

    TTask::~TTask() {
        timer_delete(JobFireTimer);
    }

    void TTask::InitializeTimer() {
        struct sigevent jobFireEvent;
        memset(&jobFireEvent, 0, sizeof(struct sigevent));
        jobFireEvent.sigev_notify = SIGEV_SIGNAL;
        jobFireEvent.sigev_signo = SIGALRM;
        timer_create(CLOCK_REALTIME, &jobFireEvent, &JobFireTimer);
    }

    void TTask::InitializeAlarmSignal() {
        sigemptyset(&AlarmSignal);
        sigaddset(&AlarmSignal, SIGALRM);
        sigprocmask(SIG_BLOCK, &AlarmSignal, NULL);
    }

    void TTask::InitializeTimerSpec() {
        long long nsInS = std::chrono::duration_cast<std::chrono::nanoseconds> (std::chrono::seconds(1)).count();
        JobFireTimeSpec.it_value.tv_sec = Offset / nsInS;
        JobFireTimeSpec.it_value.tv_nsec = Offset % nsInS;
        JobFireTimeSpec.it_interval.tv_sec = Period / nsInS;
        JobFireTimeSpec.it_interval.tv_nsec = Period % nsInS;
    }

    long long TTask::Run(long long startAt, long long endAt) {
        Offset = startAt;
        EndSimulation = std::chrono::time_point<std::chrono::high_resolution_clock>(std::chrono::nanoseconds(endAt));
        InitializeTimer();
        InitializeAlarmSignal();
        InitializeTimerSpec();
        TaskBody();
        return WorstCaseExecution;
    }

    void TTask::TaskBody() {
        timer_settime(JobFireTimer, TIMER_ABSTIME, &JobFireTimeSpec, NULL);

        WorstCaseExecution = -1;

        while (true) {
            WaitForNextActivation();
            auto start = std::chrono::high_resolution_clock::now();            
            JobBody();
            auto end = std::chrono::high_resolution_clock::now();            
            long long execution = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            //std::cout << execution << std::endl;

            WorstCaseExecution = std::max(execution, WorstCaseExecution);
            if (end > EndSimulation) {
                timer_delete(JobFireTimer);
                return;
            }

        }
    }


    void TTask::WaitForNextActivation() {
        int dummy;
        sigwait(&AlarmSignal, &dummy);
    }

    inline void TTask::JobBody() {
        for (long long i = 0; i < CountTo; ++i) {
             //Just counting
        }
    }
}