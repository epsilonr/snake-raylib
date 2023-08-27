CC := g++
FLAGS := -std=c++20 -Wall

LLIBS := -lraylib -ldl -lm -lpthread -lX11

all: build

build: main.cpp
	$(CC) $(FLAGS) $(LLIBS) main.cpp -o out && ./out