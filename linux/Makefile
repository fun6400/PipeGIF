CXX      = g++
#CXX      = clang
CXXFLAGS = -g -O2 -W -Wall -Wextra -Werror -fsanitize=address,undefined -std=c++20 -I../src $(CFLAGSX)
#g++ -Wall -Wextra -fsanitize=address,undefined -g main.cpp -o main


SRCDIR   = ../src
LOCALDIR = .

SHARED_SRCS = $(LOCALDIR)/ansiTerminal.cpp \
              $(SRCDIR)/VirtualDisplay.cpp \
              $(SRCDIR)/LzwDecode.cpp \
              $(SRCDIR)/PipeGIF.cpp

SHARED_OBJS = $(notdir $(SHARED_SRCS:.cpp=.o))

# 2. Define your individual target executables
TARGETS     = stdin2stdout array inlinestruct animation

.PHONY: all clean

# The default rule builds all targets
all: $(TARGETS)

animation: animation.o $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

stdin2stdout: stdin2stdout.o $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

array: array.o $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

inlinestruct: inlinestruct.o $(SHARED_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: $(LOCALDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(TARGETS)
