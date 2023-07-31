.PHONY: all build rebuild dvi tests gcov_report check clean cppcheck style

FILTER=

ifeq ($(OS), Linux)
	CHECK_LEAKS=CK_FORK=no valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=log.txt
	OPEN=xdg-open
else
	CHECK_LEAKS=CK_FORK=no leaks --atExit --
	FILTER=--gtest_filter=-*.Exception*
	OPEN=open
endif

all: build

rebuild: clean build

build:
	cmake -S . -B ./build -D CMAKE_CXX_COMPILER=g++
	cmake --build ./build
	
run: build
	cd build; ./Transactions

dvi:
	doxygen ./docs/Doxyfile
	$(OPEN) ./docs/html/index.html

tests: build
	cd build/tests; ./tests_transactions

style: 
	@clang-format -style=google -n -verbose */*.cc *.cc  */*.h

cppcheck: build
	cmake --build build --target cppcheck

leaks: tests
	@$(CHECK_LEAKS) ./build/tests/tests_transactions $(FILTER)

check: style cppcheck leaks

clean:
	@rm -rf ./build ./docs/html