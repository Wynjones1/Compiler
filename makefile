all: build
	cd build; make

run: all
	./bin/compiler
	./scripts/display_parse_tree.py

build:
	mkdir -p build
	cd build; cmake ..

tree: run
	./scripts/display_parse_tree.py

view: tree
	eog out.png

clean:
	rm -Rf build bin
