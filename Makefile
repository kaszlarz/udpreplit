# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = udp_graph_monitor
SOURCES = main.cpp udp_listener.cpp terminal_graph.cpp data_parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Install to system (optional)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall from system
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Test the application (requires netcat for testing)
test: $(TARGET)
	@echo "Starting UDP Graph Monitor in background..."
	@./$(TARGET) -p 4322 &
	@APP_PID=$$!; \
	sleep 2; \
	echo "Sending test data..."; \
	echo "10 20 30 25 15 35 40 30 20 25" | nc -u localhost 4322; \
	sleep 1; \
	echo "45 50 55 60 55 50 45 40 35 30" | nc -u localhost 4322; \
	sleep 1; \
	kill $$APP_PID 2>/dev/null || true

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build the application (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  uninstall- Remove from /usr/local/bin"
	@echo "  test     - Build and run a quick test"
	@echo "  help     - Show this help message"

# Declare phony targets
.PHONY: all debug clean install uninstall test help
