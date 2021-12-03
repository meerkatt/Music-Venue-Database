.DEFAULT_GOAL := test
LDFLAGS := -I src/ -I includes/ 
CXXFLAGS := -std=c++17 -stdlib=libc++ -g -O0 \
	-Wall -Wextra -Werror -Wno-unused-parameter -pedantic 
LDFLAGS := -I src/ -I includes/ 
LDLIBS := -lc++abi -lpthread -lsqlite3
CXX := clang++

test: bin/main.out bin/tests.out
	@function tearDown { kill %1; }
	@trap tearDown EXIT

	@bin/main.out&
	@sleep 2
	bin/tests.out

exec: bin/main.out
	bin/main.out

bin/main.out: src/main.cc
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $< -o $@

bin/tests.out: tests/tests.cc 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $< -o $@

includes/sqlite_orm.hpp:
	curl https://raw.githubusercontent.com/fnc12/sqlite_orm/1.6/include/sqlite_orm/sqlite_orm.h -o $@

includes/json.hpp:
	curl https://raw.githubusercontent.com/nlohmann/json/v3.9.1/single_include/nlohmann/json.hpp -o $@

includes/httplib.hpp:
	curl https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.8.4/httplib.h -o $@

compile_commands.json: Makefile clean
	bear -a make all

data/%.db: data/%.sql
	rm -f $@
	sqlite3 $@ < $<

all: bin/main.out bin/tests.out

clean:
	rm -rf bin/*
	rm data/*.db

.PHONY: clean all test exec
