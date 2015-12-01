CXXFLAGS = -Wall -O0 -std=c++11
LDFLAGS = -pthread
LDLIBS = -lm -lrt

TASK=task
SIMULATOR=simulator
SOURCE_TASK=task_main.cpp task.cpp random_var.cpp task_arg_parser.cpp
HEADERS_TASK=task.h random_var.h task_arg_parser.h

SOURCE_SIMULATOR=simulator.cpp tasks_file_parser.cpp
HEADERS_SIMULATOR=tasks_file_parser.h

all: $(TASK) $(SIMULATOR)

$(TASK): $(SOURCE_TASK) $(HEADERS_TASK)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_TASK) $(LDLIBS) -o $(TASK)

$(SIMULATOR): $(SOURCE_SIMULATOR) $(HEADERS_SIMULATOR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_SIMULATOR) $(LDLIBS) -o $(SIMULATOR)

clean:
	rm -f *.d *.o $(TASK) $(SIMULATOR)