SUCCESS_MSG        = '  [\e[32m DONE \e[0m]'

CC                 = gcc
CFLAGS             = -std=c99 -Wall -Wextra -g


all: build

build: build/libmap.o

test: build/libmap

build/libmap: build/libmap.o build/main.o
	@echo 'Building target:'
	@$(CC) $(CFLAGS) $^ -o $@
	@echo -e $(SUCCESS_MSG)

build/libmap.o: src/libmap.c src/libmap.h
	@mkdir -p build/
	@$(CC) $(CFLAGS) -c $< -o $@

build/main.o: src/main.c
	@mkdir -p build/
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo 'Cleaning workspace:'
	@rm -rf build/
	@echo -e $(SUCCESS_MSG)

rebuild: clean build
