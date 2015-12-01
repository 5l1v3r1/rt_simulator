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
    void runTask(const TTaskSpec & taskSpec, std::chrono::time_point<std::chrono::high_resolution_clock> start,
            std::chrono::time_point<std::chrono::high_resolution_clock> end) {
        size_t argc = 8 + taskSpec.ExecutionTimeSpec.size() * 2;
        char ** argv = new char * [argc];
        for (size_t i = 0; i < argc; ++i) {
            argv[i] = new char[255];
        }

        strcpy(argv[0], "task");
        strcpy(argv[1], taskSpec.Name.c_str());
        strcpy(argv[2], std::to_string(taskSpec.CPU).c_str());
        strcpy(argv[3], std::to_string(taskSpec.Priority).c_str());
        strcpy(argv[4], std::to_string(taskSpec.Period).c_str());
        strcpy(argv[5], std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>
                                        (start.time_since_epoch()).count()).c_str());
        strcpy(argv[6], std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>
                                        (end.time_since_epoch()).count()).c_str());
        strcpy(argv[7], std::to_string(taskSpec.ExecutionTimeSpec.size()).c_str());

        for (size_t massNumber = 0; massNumber < taskSpec.ExecutionTimeSpec.size(); ++massNumber) {
            strcpy(argv[8 + 2 * massNumber], std::to_string(taskSpec.ExecutionTimeSpec[massNumber].first).c_str());
            strcpy(argv[9 + 2 * massNumber], std::to_string(taskSpec.ExecutionTimeSpec[massNumber].second).c_str());
        }

    	int child = fork();
	    if (child < 0) {
	        std::cerr << taskSpec.Name << ": can`t create new process." << std::endl;
	        exit(-1);
	    }
	    if (child == 0) {
	        int res = execve("./task", argv, NULL);
	        if (res < 0) {
		        std::cerr << taskSpec.Name << "Error can`t run task" << std::endl;
		        exit(-1);
	    	}
	    }
        

        for (size_t i = 0; i < argc; ++i) {
            delete [] argv[i];
        }
        delete [] argv;
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
    std::vector<NRTSimulator::TTaskSpec> taskSpecs = NRTSimulator::TTaskFileParser().Parse(taskSpecFile);
    taskSpecFile.close();

    std::cout << "Responce time analysis..." << std::endl;

    NRTSimulator::TRTA rta(taskSpecs);
    rta.Compute();

    for (size_t taskNumber = 0; taskNumber < taskSpecs.size(); ++ taskNumber) {
        if (rta.CheckIsShedulable(taskNumber)) {
            std::cout << taskSpecs[taskNumber].Name << ": worst case responce time " 
                << rta.GetWorstCaseResponceTime(taskNumber) << std::endl;            
        } else {
            std::cout << taskSpecs[taskNumber].Name << ": is not schedulable" << std::endl;
        }
    }

    std::cout << "Simulation..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now() + TASK_OFFSET;
    auto end = start + std::chrono::seconds(simulationTime);    
    for (const auto & taskSpec: taskSpecs) {
        NRTSimulator::runTask(taskSpec, start, end);
    }
    
    int dummy;
    for (size_t proceesNumber = 0; proceesNumber < taskSpecs.size(); ++ proceesNumber) {
        wait(&dummy);
    }

    return 0;
}
