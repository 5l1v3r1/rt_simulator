CXXFLAGS = -Wall -O0 -std=c++11
LDFLAGS = -pthread
LDLIBS = -lm -lrt -lconfig++

SIMULATOR=simulator

SOURCE_SIMULATOR=simulator.cpp tasks_file_parser.cpp task.cpp random_var.cpp rta.cpp cmd_arg_parser.cpp cdf_plot.cpp latency_task.cpp

all: $(SIMULATOR)

$(SIMULATOR): $(SOURCE_SIMULATOR:.cpp=.o)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE_SIMULATOR:.cpp=.o) $(LDLIBS) -o $(SIMULATOR)

clean:
	rm -f *.d *.o $(SIMULATOR)

indent:
	astyle --style=kr --pad-header --indent=tab --indent-namespaces --break-blocks\
		--pad-oper --unpad-paren --add-one-line-brackets \
		--suffix=none --max-code-length=80 --break-after-logical  *.cpp *.h

###################################
%.d: SOURCE_SIMULATOR
	$(CXX) -MM -MF $@ $<

%.o: %.d

-include $(SOURCE_SIMULATOR:.cpp=.d)
