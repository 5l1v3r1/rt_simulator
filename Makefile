CXXFLAGS = -Wall -O0 -std=c++11
LDFLAGS = -pthread
LDLIBS = -lm -lrt

APP=rt_simulator
#OBJS=task_main.o task.o
SOURCE=task_main.cpp task.cpp
HEADERS=task.h

all: $(SOURCE) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCE) $(LDLIBS) -o $(APP)

$(APP): $(OBJS)

clean:
	rm -f *.d *.o $(APP)

###################################
# %.d: %.cpp
# 	$(CXX) -MM -MF $@ $< $(CXXFLAGS)
# %.o: %.d

# -include $(OBJS:.o=.d)

