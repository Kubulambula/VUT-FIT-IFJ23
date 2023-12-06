TEAM_NAME = xnecas31

CC=gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -DNDEBUG
CFLAGS_WERROR = -std=c99 -Wall -Wextra -Werror -pedantic
CFLAGS_DEBUG = -std=c99 -Wall -Wextra -pedantic
CFLAGS_VERBOSE = -std=c99 -Wall -Wextra -pedantic -DVERBOSE

EXECUTABLE = ./ifj23compiler

TEST_DIR = .testdir


# make - compiles ifj23compiler
default:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) ./src/*.c

src:
	$(CC) $(CFLAGS) -o $(EXECUTABLE) ./src/*.c

# make debug - just like make, but compiles with debug
debug:
	$(CC) $(CFLAGS_DEBUG) -o $(EXECUTABLE) ./src/*.c


# same as debug, but handles warning as errors
werror:
	$(CC) $(CFLAGS_WERROR) -o $(EXECUTABLE) ./src/*.c


# same as debug, but prints more stuff
verbose:
	$(CC) $(CFLAGS_VERBOSE) -o $(EXECUTABLE) ./src/*.c


run:
	clear ; make && ./$(EXECUTABLE) < test.ifj23 > code.ifjcode && echo ===== Copiled program output ===== && ./external/ic23int_linux/ic23int ./code.ifjcode && echo

run-int:
	clear ; echo ===== Copiled program output ===== && ./external/ic23int_linux/ic23int ./code.ifjcode && echo


# make test - runs all tests
test:
	./tests/test.py -d ./tests/tests/ -c ./ifj23compiler -i ./external/ic23int_linux/ic23int


# make zip - zips all required resources in .zip file
zip:
	zip -j $(TEAM_NAME).zip Makefile src/*.c src/*.h doc/dokumentace.pdf rozdeleni rozsireni


# make is_is_ok - runs is_it_ok.sh
is_it_ok: zip
	yes | ./external/is_it_ok.sh $(TEAM_NAME).zip $(TEST_DIR)
