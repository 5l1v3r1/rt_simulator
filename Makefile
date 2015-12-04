CXXFLAGS = -Wall -O0 -std=c++11
LDFLAGS = -pthread
LDLIBS = -lm -lrt -lconfig++

SIMULATOR=simulator

SOURCE_SIMULATOR=simulator.cpp tasks_file_parser.cpp task.cpp random_var.cpp rta.cpp

all: $(SIMULATOR)

$(SIMULATOR): $(SOURCE_SIMULATOR:.cpp=.o)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_SIMULATOR:.cpp=.o) $(LDLIBS) -o $(SIMULATOR)

clean:
	rm -f *.d *.o $(SIMULATOR)

###################################
%.d: SOURCE_SIMULATOR
	$(CXX) -MM -MF $@ $<

%.o: %.d

-include $(SOURCE_SIMULATOR:.cpp=.d)
