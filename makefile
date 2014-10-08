all: build
	cd build; make

run: all
	./bin/compiler -i ./tests/all.c
	./scripts/display_parse_tree.py

build:
	mkdir -p build
	cd build; cmake ..

cov: all
	cd build; make coverage

tree:
	./scripts/display_parse_tree.py

view: tree
	eog out.png

clean:
	rm -Rf build bin tests/*.list
