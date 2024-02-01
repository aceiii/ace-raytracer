.PHONY: run clean

all: main

main: main.cpp colour.h hittable.h hittable_list.h interval.h rtweekend.h sphere.h vec3.h
	g++ -std=c++17 main.cpp -o main

run: main
	./main > output.ppm

clean:
	rm main output.ppm
