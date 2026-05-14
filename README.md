# CPU Scheduling Policies Simulator

C++ simulator for classic operating-system CPU scheduling policies. The program reads scheduling scenarios from standard input, runs the selected algorithm, and prints trace/statistics output that is validated against bundled test cases.

## Hiring Signal

- Implements multiple scheduling algorithms in one codebase rather than a single hard-coded policy.
- Produces both execution traces and aggregate statistics for turnaround / normalized turnaround analysis.
- Includes a portable fixture-test runner with many input/output cases.
- Uses Docker support for a reproducible Linux-style build environment.

## Implemented Policies

- First Come First Served (FCFS)
- Round Robin (RR)
- Shortest Process Next (SPN)
- Shortest Remaining Time (SRT)
- Feedback Queue (FB)
- Feedback Queue with exponential quantum (`FB-2i`)
- Aging

## Repository Structure

```text
.
|-- lab6.cpp        # Scheduler implementation
|-- makefile        # Build and fixture-test commands
|-- scripts/        # Portable Python test runner
|-- Dockerfile      # Reproducible build container
|-- testcases/      # Input/output validation fixtures
`-- LICENSE
```

## Build

```bash
make build
```

This compiles `lab6.cpp` into `./lab6`.

## Docker Build

Docker is optional. Use it only when Docker Desktop or a Linux Docker daemon is running:

```bash
make docker-build
make docker
```

## Run Tests

```bash
make test
```

The test target pipes each testcase into the compiled scheduler and compares output with the expected fixture.
The comparison is implemented in `scripts/run_fixture_tests.py`, so it works in PowerShell, Linux shells, and WSL.

## Clean

```bash
make clean
```
