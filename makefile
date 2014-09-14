all: build
	cd build; make

run: all
	./bin/compiler -i ./tests/fib.c

build:
	mkdir -p build
	cd build; cmake ..

tree: run
	./scripts/display_parse_tree.py

clean:
	rm -Rf build bin
