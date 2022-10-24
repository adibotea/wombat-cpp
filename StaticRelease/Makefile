
all: wombat 

CXXFLAGS=-mcmodel=medium -O3 -g -DNDEBUG -pthread -std=c++11 -static
#CXXFLAGS=-mcmodel=medium -O3 -g -DNDEBUG -pthread -std=c++11 -Weffc++
#CXXFLAGS=-mcmodel=medium -O3 -g -DNDEBUG -pthread -std=c++11
#CXXFLAGS=-mcmodel=medium -O0 --no-inline  -g -lpthread -std=c++11

WOMBAT_SRCS= booleanmap.cpp \
 deadlockrecord.cpp \
 randomwritestate.cpp staticslotstats.cpp \
 wordslot.cpp cell.cpp depthfirstsearch.cpp \
 multicharmap.cpp search.cpp \
 statscollection.cpp cellrecord.cpp \
 dictionary.cpp multicharpattern.cpp stats.cpp \
 fixedlengthdictionary.cpp \
 node.cpp state.cpp util.cpp \
 deadlockdatabase.cpp globals.cpp \
 parammanager.cpp staticpuzzleinfo.cpp \
 wordpattern.cpp limiteddiscrsearch.cpp bestfirstsearch.cpp dfbnb.cpp

WOMBAT_OBJS1= $(patsubst %.cpp, %.o, $(WOMBAT_SRCS))
WOMBAT_OBJS= $(WOMBAT_SRCS:%.cpp=obj/%.o)
NRPA_SRCS= Experiment.cpp 
NRPA_OBJS= $(patsubst %.cpp, obj/%.o, $(NRPA_SRCS))



obj/%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

obj/Experiment.o: ../src/Experiment.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

wombat: ../src/main.o $(NRPA_OBJS) $(WOMBAT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(NRPA_OBJS) $(WOMBAT_OBJS)

clean:
	rm -rf *~ $(WOMBAT_OBJS) $(NRPA_OBJS)

deps:
	$(CXX) $(CXXFLAGS) -MM $(NRPA_SRCS) $(WOMBAT_SRCS)

test:
	make -C test simple-test

main.o: main.cpp

