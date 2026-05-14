all: build

.PHONY: all build docker-build docker test lab6 clean

build: lab6.cpp
	@g++ -o lab6 lab6.cpp

docker-build: build
	@docker build -t cpp-scheduler .

docker: docker-build
	@docker run -it --rm cpp-scheduler

test: build
	@python scripts/run_fixture_tests.py

lab6: test

clean:
	@rm -f lab6
