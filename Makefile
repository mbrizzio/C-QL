# Compiler and flags
CXX = g++-11
CXXFLAGS = -std=c++20 -Wall -Wextra
DEBUGFLAGS = -g

# Executable names
RELEASE_TARGET = sql.exe
DEBUG_TARGET = sqldebug.exe

# Source files
SRCS = main.cpp datatypes.cpp column.cpp
HDRS = datatypes.h column.h testsuite.h

# Object files
RELEASE_OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=.debug.o)

# Default target
all: $(RELEASE_TARGET)

# Release build
$(RELEASE_TARGET): $(RELEASE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Debug build
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $@ $^

# Compile .cpp to .o (release)
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile .cpp to .debug.o (debug)
%.debug.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Clean all builds
clean:
	rm -f *.o *.debug.o $(RELEASE_TARGET) $(DEBUG_TARGET)