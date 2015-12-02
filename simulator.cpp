#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string.h>
#include <vector>
#include <fstream>
#include <getopt.h>

#include "tasks_file_parser.h"
#include "rta.h"


namespace NRTSimulator {
    struct TTreadParams {
        long long Start;
        long long End;
        std::shared_ptr<TTask> Task;
    };
    static void * runTask (void * params)
    {
        TTreadParams * paramsTyped = (TTreadParams *)params;

        paramsTyped->Task->Run(paramsTyped->Start, paramsTyped->End);

        return NULL;
    }

    void parseCommandLineArgs(int argc,char * argv[], std::string & filename, int & simulationTime) {
        const std::string USAGE("Usage: simulator -f str -s num.\n");

        static struct option long_options[] = {
            {"file",    required_argument, 0, 'f'},
            {"simtime", required_argument, 0, 's'},
            {0, 0, 0,  0}
        };

        int long_index = 0;
        int opt = 0;   
        while ((opt = getopt_long(argc, argv,"f:s:", long_options, &long_index )) != -1) {
            switch (opt) {
                case 'f' : 
                    filename = std::string(optarg);
                    break;
                case 's' :
                    simulationTime = std::stoll(optarg);
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
}

const std::chrono::seconds TASK_OFFSET(2);
int main(int argc, char *argv[])
{
    int simulationTime = 0;
    std::string filename;

    NRTSimulator::parseCommandLineArgs(argc, argv, filename, simulationTime);

    std::ifstream taskSpecFile(filename);
    std::vector<std::shared_ptr<NRTSimulator::TTask>> tasks = NRTSimulator::TTaskFileParser().Parse(taskSpecFile);
    taskSpecFile.close();

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
    }

    return 0;
}
