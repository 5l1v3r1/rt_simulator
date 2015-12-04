#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string.h>
#include <vector>
#include <getopt.h>

#include "tasks_file_parser.h"
#include "rta.h"

namespace NRTSimulator {
    struct TTreadParams {
        long long Start;
        long long End;
        std::shared_ptr<TTask> Task;
    };
    static void * runTask (void * params) {
        TTreadParams * paramsTyped = (TTreadParams *)params;
        paramsTyped->Task->Run(paramsTyped->Start, paramsTyped->End);
        return NULL;
    }

    static void parseCommandLineArgs(int argc,char * argv[], std::string & filename, int & simulationTime,
        bool & counting, bool & hist) {
        //TODO: create class for parsing command line args
        
        const std::string USAGE("Usage: simulator [pc] -f str -s num.\n");
        counting = false;
        hist = false;
        //TODO: add kernel latency parameter, add output parameter
        //TODO: create default for file and simtime
        //TODO: add help argument
        static struct option long_options[] = {
            {"file",    required_argument, 0, 'f'},
            {"simtime", required_argument, 0, 's'},
            {"plot", no_argument, 0, 'p'},
            {"counting", no_argument, 0, 'c'},
            {0, 0, 0,  0}
        };

        int long_index = 0;
        int opt = 0;   
        while ((opt = getopt_long(argc, argv,"pcf:s:", long_options, &long_index )) != -1) {
            switch (opt) {
                case 'f' : 
                    filename = std::string(optarg);
                    break;
                case 's' :
                    simulationTime = std::stoll(optarg);
                    break;
                case 'c':
                    counting = true;
                    break;
                case 'p':
                    hist = true;
                    break;
                default: 
                    std::cout << USAGE << std::endl;
                    exit(-1);
            }
        }

        if (filename.empty() || simulationTime == 0) {
            std::cout << USAGE << std::endl;
            exit(-1);
        }
    }

    static void plotResponceTimeHistogramm(const std::shared_ptr<TTask> task) {
        //TODO: create class for ploting
        std::vector<long long> responceTimes = task->GetResponceTimes();

        FILE* pipehandle=popen("gnuplot -persist","w"); 
        fprintf(pipehandle, "set term wxt title '%s Responce Time'\n", task->GetName().c_str());
        fprintf(pipehandle, "set xlabel 'ResponceTime (ms)'\n");
        fprintf(pipehandle, "set ylabel 'CDF'\n");
        fprintf(pipehandle, "plot \"-\" u 1:(1./%lu) smooth cumulative\n", responceTimes.size());

        
        for (const auto & responceTime : responceTimes) {
            std::fprintf(pipehandle, "%f\n", responceTime / 1000000.0);
        }
        fprintf(pipehandle, "e\n");
        fprintf(pipehandle,"quit\n");
        fflush(pipehandle);
        fclose(pipehandle);
    }

}

static const std::chrono::seconds TASK_OFFSET(2);
int main(int argc, char *argv[]) {
    int simulationTime = 0;
    std::string filename;
    bool histogramm;
    bool counting;

    NRTSimulator::parseCommandLineArgs(argc, argv, filename, simulationTime, counting, histogramm);

    std::vector<std::shared_ptr<NRTSimulator::TTask>> tasks = NRTSimulator::TTaskFileParser(!counting).Parse(filename);

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
    auto end = start + std::chrono::seconds(simulationTime);    

    std::vector<pthread_t> threads(tasks.size());
    std::vector<NRTSimulator::TTreadParams> task_params(tasks.size());
    for (size_t i = 0; i < tasks.size(); ++i) {        
        task_params[i].Task = tasks[i];
        task_params[i].Start = std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count();
        task_params[i].End = std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count();
        pthread_create(&threads[i], NULL, &NRTSimulator::runTask, &task_params[i]);
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

    void ** dummy = NULL;
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], dummy);
        std::cout << tasks[i]->GetName() << ": worst case responce time " << tasks[i]->GetWorstCaseResponceTime() << std::endl;
        if (histogramm) {
            NRTSimulator::plotResponceTimeHistogramm(tasks[i]);
        }
    }

    std::cout << "Worst case kernell latency: " << rta.EstimateWorstCaseKernellLatency() << std::endl;

    return 0;
}
