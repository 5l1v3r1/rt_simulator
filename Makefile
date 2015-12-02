CXXFLAGS = -Wall -O0 -std=c++11
LDFLAGS = -pthread
LDLIBS = -lm -lrt

SIMULATOR=simulator

SOURCE_SIMULATOR=simulator.cpp tasks_file_parser.cpp task.cpp random_var.cpp rta.cpp
HEADERS_SIMULATOR=tasks_file_parser.h random_var.h task.h rta.h

all: $(SIMULATOR)

# $(TASK): $(SOURCE_TASK) $(HEADERS_TASK)
# 	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_TASK) $(LDLIBS) -o $(TASK)

$(SIMULATOR): $(SOURCE_SIMULATOR) $(HEADERS_SIMULATOR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_SIMULATOR) $(LDLIBS) -o $(SIMULATOR)

clean:
	rm -f *.d *.o $(SIMULATOR)