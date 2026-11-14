CXX = g++
CXXFLAGS = -std=c++17
TARGET = snake
SRC = snake.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) && echo "Build successful: $(TARGET)" || (echo "Build failed." && exit 1)

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -f $(TARGET)
