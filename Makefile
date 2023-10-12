TEAM_NAME = xnecas31

CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -DNDEBUG
CFLAGS_DEBUG = -std=c99 -Wall -Wextra -pedantic

EXECUTABLE = ifj23compiler

TEST_DIR = .testdir


# make - compiles ifj23compiler
default:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) ./src/*.c


# make debug - just like make, but compiles with debug
debug:
	$(CC) $(CFLAGS_DEBUG) -o $(EXECUTABLE) ./src/*.c


# make test - runs all tests
test:
	./tests/test.py -d ./tests/tests/ -c ./ifj23compiler -i ./ic23int_linux/ic23int


# make zip - zips all required resources in .zip file
zip:
	zip -j $(TEAM_NAME).zip Makefile src/*.c src/*.h doc/dokumentace.pdf rozdeleni rozsireni


# make is_is_ok - runs is_it_ok.sh
is_it_ok: zip
	./external/is_it_ok.sh $(TEAM_NAME).zip $(TEST_DIR)
