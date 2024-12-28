# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread

# Source files
SRC = bank_main.cpp ATM.cpp Bank.cpp ReaderWriter.cpp accounts.cpp Logger.cpp globals.cpp
# Object files
OBJ = $(SRC:.cpp=.o)
# Output executable
TARGET = bank

# Default target: build the project
all: $(TARGET)

# Rule to build the final executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Rule to build object files
%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean

