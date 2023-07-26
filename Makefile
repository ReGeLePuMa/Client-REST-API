CC = g++
CFLAGS = -Wall -Wextra -g -std=c++17 -lm

.PHONY: build run clean

build: client

client: client.cpp requests.hpp buffer.hpp nlohmann/json.hpp
	$(CC) -o $@ $@.cpp $(CFLAGS)

run:
	./client

clean:
	rm -rf *.o client	

	

