#include <iostream>
#include <chrono>
#include <vector>

#include "task.h"
#include "tasks_file_parser.h"
#include "rta.h"
#include "cmd_arg_parser.h"
#include "cdf_plot.h"


static const std::chrono::seconds TASK_OFFSET(2);
int main(int argc, char *argv[]) {
    NRTSimulator::TCmdArgParser argParser(argc, argv);

    std::vector<std::shared_ptr<NRTSimulator::TTask>> tasks =
         NRTSimulator::TTaskFileParser(!argParser.IsCountingUsed()).Parse(argParser.GetTaskSpecFileName());

    if (argParser.IsCountingUsed()) {
        std::cout << "Estimate convert rate for counting task..." << std::endl;
        NRTSimulator::TCountingTask::EstimateConvertRate();
    }

    std::cout << "Responce time analysis..." << std::endl;

    NRTSimulator::TRTA rta(tasks);
    rta.Compute();

    for (size_t taskNumber = 0; taskNumber < tasks.size(); ++ taskNumber) {
        if (rta.CheckIsShedulable(taskNumber)) {
            std::cout << tasks[taskNumber]->GetName() << ": worst case responce time " 
                << rta.GetWorstCaseResponceTime(taskNumber) << std::endl;            
        } else {
            std::cout << tasks[taskNumber]->GetName() << ": is not schedulable" << std::endl;
        }
    }

    std::cout << "Simulation..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now() + TASK_OFFSET;
    auto end = start + std::chrono::seconds(argParser.GetSimulationTime());    

    std::vector<pthread_t> threads(tasks.size());
    std::vector<NRTSimulator::TTaskTreadParams> task_params(tasks.size());
    for (size_t i = 0; i < tasks.size(); ++i) {
        tasks[i]->Run(std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count(),
                    std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count());
    }   

    // TODO: create class representing high kernel latency task
    // cpu_set_t set;
    // CPU_ZERO(&set);
    // CPU_SET(0, &set);

    // if (pthread_setaffinity_np(pthread_self(), sizeof(set), &set) == -1) {
    //     std::cerr << "failed to set cpu." << std::endl;
    //     exit(-1);
    // } 

    // while(std::chrono::high_resolution_clock::now() < end) {
    //     int p = fork();
    //     if (p == 0) {
    //         sleep(100);
    //     } else {
    //         kill(p, SIGTERM);
    //     }
    //     usleep(1000);
    // }

    for (size_t i = 0; i < threads.size(); ++i) {
        tasks[i]->Join();
        std::cout << tasks[i]->GetName() << ": worst case responce time " << tasks[i]->GetWorstCaseResponceTime() << std::endl;
    }
    if (argParser.IsPlotNeeded()) {
        std::cout << "Ploting..." << std::endl;
        for (const auto & task : tasks) {
            NRTSimulator::TCDFPlot().Plot(task->GetResponceTimes(), task->GetName());
        }
    }
    if (argParser.IsHighKernelLatencyNeeded()) {
        std::cout << "Worst case kernell latency: " << rta.EstimateWorstCaseKernellLatency() << std::endl;
    }    
    //TODO: Create class that write output (Check if directory exists in cmd_arg_parser)
    return 0;
}
