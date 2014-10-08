all: build
	cd build; make

run: all
	./bin/compiler -i ./tests/success/all.c
	./scripts/display_parse_tree.py

build:
	mkdir -p build
	cd build; cmake ..

cov: tests
	cd build; make coverage

tree:
	./scripts/display_parse_tree.py

build_tests:
	mkdir -p build
	cd  build; cmake .. -DRUN_TESTS=on

tests: build_tests
	cd build; make
	./bin/compiler

view: tree
	eog out.png

clean:
	rm -Rf build bin tests/*.list *.png *.tree
