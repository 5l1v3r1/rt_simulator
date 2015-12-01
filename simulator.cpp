#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <chrono>
#include <sstream>
#include <string.h>


namespace NRTSimulator {
    void runTask(char * const argv[]) {
    	int child = fork();
	    if (child < 0) {
	        std::cerr << "Can`t run task." << std::endl;
	        exit(-1);
	    }
	    if (child == 0) {
	        int res = execve("./task", argv, NULL);
	        if (res < 0) {
		        std::cerr << "Error can`t run task" << std::endl;
		        exit(-1);
	    	}
	    }
    }
}

int main(int argc, char *argv[])
{
    //TODO: remove this part
    const int arg_count = 16;
    char * args[arg_count];
    for (int i = 0; i < arg_count; ++i) {
        args[i] = new char[255];
    }
    strcpy(args[0], "task");
    strcpy(args[1], "T1");
    strcpy(args[2], "0");
    strcpy(args[3], "80");
    strcpy(args[4], "40000000");
    std::stringstream s;
    s << std::chrono::duration_cast<std::chrono::nanoseconds>
                    ((std::chrono::high_resolution_clock::now() + std::chrono::seconds(2)).time_since_epoch()).count();
    strcpy(args[5], s.str().c_str());
    std::stringstream e;
    e << std::chrono::duration_cast<std::chrono::nanoseconds>
                    ((std::chrono::high_resolution_clock::now() + std::chrono::seconds(62)).time_since_epoch()).count();
    strcpy(args[6], e.str().c_str());

    strcpy(args[7], "4");
    strcpy(args[8], "0.1");
    strcpy(args[9], "10000000");
    strcpy(args[10], "0.4");
    strcpy(args[11], "10000000");
    strcpy(args[12], "0.2");
    strcpy(args[13], "10000000");
    strcpy(args[14], "0.3");
    strcpy(args[15], "10000000");

    //////////////////////////////

    NRTSimulator::runTask(args);
    strcpy(args[1], "T2");
    strcpy(args[3], "81");
    NRTSimulator::runTask(args);
    strcpy(args[1], "T3");
    strcpy(args[3], "82");
    NRTSimulator::runTask(args);

    int dummy;
    wait(&dummy);
    wait(&dummy);
    wait(&dummy);

    return 0;
}
