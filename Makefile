
main: build/main
	cp ./build/main main

build/main: build
	$(MAKE) rebuild

rebuild:
	cmake --build ./build

build:
	cmake -B build -S . -G Ninja

clean:
	cmake --build ./build --target clean

run: main
	./main


.PHONY: run  rebuild clean
