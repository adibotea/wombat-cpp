
all: wombat 

CXXFLAGS=-mcmodel=medium -O0 --no-inline -g -pthread -std=c++11
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
 Experiment.cpp \
 parammanager.cpp staticpuzzleinfo.cpp \
 wordpattern.cpp limiteddiscrsearch.cpp bestfirstsearch.cpp dfbnb.cpp

WOMBAT_OBJS= $(WOMBAT_SRCS:%.cpp=obj/%.o)



obj/%.o: ../src/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

wombat: ./obj/main.o $(WOMBAT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $< $(WOMBAT_OBJS)

clean:
	rm -rf *~ $(WOMBAT_OBJS) ../obj/main.o

deps:
	$(CXX) $(CXXFLAGS) -MM $(WOMBAT_SRCS)

obj/main.o: ../src/main.cpp
