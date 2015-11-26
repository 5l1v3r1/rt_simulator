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

#include "task.h"


namespace NRTSimulator {
    TTask::TTask(long long executionTime, long long period, long long offset, double convertRate)
        : ExecutionTime(executionTime)
        , Period(period)
        , Offset(offset)
    {
        CountTo = executionTime / convertRate;
        std::cout << CountTo << std::endl;
    }

    TTask::~TTask(){};

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
        JobDoneTimeSpec.it_value.tv_sec = Offset / nsInS;
        JobDoneTimeSpec.it_value.tv_nsec = Offset % nsInS;
        JobDoneTimeSpec.it_interval.tv_sec = Period / nsInS;
        JobDoneTimeSpec.it_interval.tv_nsec = Period % nsInS;
    }

    void TTask::Run() {
        std::cout << "Task Started" << std::endl;
        InitializeTimer();
        InitializeAlarmSignal();
        InitializeTimerSpec();
        TaskBody();
    }

    void TTask::TaskBody() {
        timer_settime(JobFireTimer, TIMER_ABSTIME, &JobDoneTimeSpec, NULL);

        while (true) {
            WaitForNextActivation();
            auto start = std::chrono::high_resolution_clock::now();
            JobBody();
            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
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