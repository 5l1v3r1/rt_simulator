#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string.h>
#include <vector>
#include <fstream>

#include "tasks_file_parser.h"


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

}

const std::chrono::seconds TASK_OFFSET(2);
int main(int argc, char *argv[])
{   
    int executionTime = 5;
    std::ifstream taskSpecFile("task_spec.txt");
    std::vector<NRTSimulator::TTaskSpec> taskSpecs = NRTSimulator::TTaskFileParser().Parse(taskSpecFile);


    taskSpecFile.close();
    auto start = std::chrono::high_resolution_clock::now() + TASK_OFFSET;
    auto end = start + std::chrono::seconds(executionTime);
    
    for (const auto & taskSpec: taskSpecs) {
        NRTSimulator::runTask(taskSpec, start, end);
    }
    
    std::cout << "All tasks started." << std::endl;

    int dummy;
    for (size_t proceesNumber = 0; proceesNumber < taskSpecs.size(); ++ proceesNumber) {
        wait(&dummy);
    }

    return 0;
}
