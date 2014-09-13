all: build
	cd build; make

run: all
	./bin/compiler

build:
	mkdir -p build
	cd build; cmake ..

clean:
	rm -Rf build bin
