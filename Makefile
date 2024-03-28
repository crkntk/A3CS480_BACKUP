# CXX Make variable for compiler
CXX=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = Map.o LoadedPage.o PageReplacement.o PageTable.o PageNode.o log_helpers.o vaddr_tracereader.o main.o

# Program name
PROGRAM = pagingwithpr

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS)
	$(CXX) -o $(PROGRAM) $^

PageNode.o : PageNode.h PageNode.cpp
	$(CXX) $(CXXFLAGS) PageNode.cpp
LoadedPage.o : LoadedPage.h LoadedPage.cpp
	$(CXX) $(CXXFLAGS) LoadedPage.cpp
PageReplacement.o : PageReplacement.h PageReplacement.cpp
	$(CXX) $(CXXFLAGS) PageReplacement.cpp
Map.o : Map.h Map.cpp
	$(CXX) $(CXXFLAGS) Map.cpp
log_helpers.o : log_helpers.h log_helpers.cpp
	$(CXX) $(CXXFLAGS) log_helpers.cpp
vaddr_tracereader.o : vaddr_tracereader.h vaddr_tracereader.cpp
	$(CXX) $(CXXFLAGS) vaddr_tracereader.cpp
PageTable.o : PageTable.h PageTable.cpp
	$(CXX) $(CXXFLAGS) PageTable.cpp

main.o : PageTable.h main.cpp
	$(CXX) $(CXXFLAGS) main.cpp
clean :
	rm -f *.o $(PROGRAM)

