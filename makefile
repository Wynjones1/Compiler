CC        := clang
SRC       := main.c compiler.c lexer.c parser.c symbol.c ast.c
OBJ       := $(patsubst %.c, %.o, $(SRC))
EXE       := compiler
OPT_LEVEL := 0
CFLAGS    := -O$(OPT_LEVEL) -g -Wall -Wno-unused -Werror --std=c11

.PHONY : pre

all: pre compile

compile: $(OBJ)
	$(CC) $^ -o $(EXE) $(CFLAGS)

pre:
	@echo "CFLAGS = $(CFLAGS)"

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $< -c -o $@ $(CFLAGS)

run: all
	./$(EXE)

clean:
	rm -rf *.o $(EXE)
