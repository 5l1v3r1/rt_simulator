#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string.h>
#include <vector>
#include <getopt.h>

#include "tasks_file_parser.h"
#include "rta.h"

#include <sys/types.h>
#include <unistd.h>

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
        
        const std::string USAGE("Usage: simulator [ch] -f str -s num.\n");
        counting = false;
        hist = false;
        static struct option long_options[] = {
            {"file",    required_argument, 0, 'f'},
            {"simtime", required_argument, 0, 's'},
            {"hist", no_argument, 0, 'h'},
            {"counting", no_argument, 0, 'c'},
            {0, 0, 0,  0}
        };

        int long_index = 0;
        int opt = 0;   
        while ((opt = getopt_long(argc, argv,"hcf:s:", long_options, &long_index )) != -1) {
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
                case 'h':
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
        std::vector<long long> responceTimes = task->GetResponceTimes();
        int numberOfIntervals = 10;
        double min = *std::max_element(responceTimes.begin(), responceTimes.end()) / 1000000.0;
        double max = *std::min_element(responceTimes.begin(), responceTimes.end()) / 1000000.0;
        double width = (max - min) / numberOfIntervals;

        FILE* pipehandle=popen("gnuplot -persist","w"); 
        fprintf(pipehandle, "set term wxt title '%s Responce Time'\n", task->GetName().c_str());
        fprintf(pipehandle, "set xlabel 'ResponceTime (ms)'\n");
        fprintf(pipehandle, "set ylabel 'Count'\n");
        fprintf(pipehandle, "width=%f\n", width);
        fprintf(pipehandle, "hist(x,width)=width*floor(x/width)+width/2.0\n"); 
        fprintf(pipehandle, "set boxwidth width*0.9\n");        
        fprintf(pipehandle, "plot \"-\" u (hist($1,width)):(1.0) smooth freq w boxes lc rgb\"green\" notitle\n");

        
        for (const auto & responceTime : responceTimes) {
            std::fprintf(pipehandle, "%f\n", responceTime / 1000000.0);
        }
        fprintf(pipehandle, "e\n");
        fprintf(pipehandle,"quit\n");
        fflush(pipehandle);
        fclose(pipehandle);
    }

}

const std::chrono::seconds TASK_OFFSET(2);
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

    void ** dummy = NULL;
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], dummy);
        std::cout << tasks[i]->GetName() << ": worst case responce time " << tasks[i]->GetWorstCaseResponceTime() << std::endl;
        if (histogramm) {
            NRTSimulator::plotResponceTimeHistogramm(tasks[i]);
        }
    }

    return 0;
}
