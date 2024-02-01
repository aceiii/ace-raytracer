.PHONY: run clean


all: main

main:
	g++ -std=c++17 main.cpp -o main

run: main
	./main > output.ppm

clean:
	rm main output.ppm

