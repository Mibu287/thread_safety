CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -Wthread-safety

CLANG_TIDY = clang-tidy
CLANG_TIDY_CHECKS = cppcoreguidelines-*,-header-filter=.*

PWD:=$(shell pwd)

.DEFAULT_GOAL:= run
.PHONY: run
run: demo.out
	@${PWD}/$<

%.out: %.o
	${LINK.cc} -o $@ $<

%.o: %.cc *.h
	${COMPILE.cc} -o $@ $<

tidy: *.h
tidy: demo.cc
	${CLANG_TIDY} $(filter %.c %.cc %.cpp, $^) -checks="${CLANG_TIDY_CHECKS}" -- ${CPPFLAGS} ${CXXFLAGS}
	@echo "Placeholder for tidy receipe in Makefile" > $@

.PHONY: clean
clean:
	${RM} *.out *.o tidy
