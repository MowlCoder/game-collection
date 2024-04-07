CXX := g++
OUTPUT := sfmlgame

SFML_DIR := .

CXX_FLAGS := -O3 -std=c++20
INCLUDES := -I./src -I ./src/imgui -I$(SFML_DIR)/include
LDFLAGS := -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lGL -L$(SFML_DIR)/lib

SRC_FILES := $(wildcard src/*.cpp src/imgui/*.cpp)
OBJ_FILES := $(SRC_FILES:.cpp=.o)

all:$(OUTPUT)

$(OUTPUT):$(OBJ_FILES) Makefile
		$(CXX) $(OBJ_FILES) $(LDFLAGS) -o ./bin/$@

.cpp.o:
		$(CXX) -c $(CXX_FLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJ_FILES) ./bin/sfmlgame

run: $(OUTPUT)
	./bin/sfmlgame
