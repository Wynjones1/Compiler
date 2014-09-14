all: build
	cd build; make

run: all
	./bin/compiler

build:
	mkdir -p build
	cd build; cmake ..

tree: run
	./scripts/display_parse_tree.py

clean:
	rm -Rf build bin
