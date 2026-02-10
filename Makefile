CXX = g++-15
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
LDFLAGS = -lSDL2 -lSDL2_ttf

SRC = Body.cpp Game.cpp main.cpp 
OBJ = $(SRC:.cpp=.o)
TARGET = massatraction

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)
	@echo "+ Build complete."

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
