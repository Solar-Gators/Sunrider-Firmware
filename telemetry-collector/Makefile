CPP_FILES = src/lib/*.cpp
CPP_FILES += ../car-bsp/DataModules/src/*.cpp
CPP_FILES += src/main.cpp

CPP_INCLUDES = -I ../car-bsp/DataModules/inc
CPP_INCLUDES += -I ../car-bsp/etl/incude
CPP_INCLUDES += -I ./src/lib/inc


EXECUTABLE = collector

BASE_COMPILE = g++ -std=c++11 -D IS_TELEMETRY $(shell python3-config --cflags --embed) -o $(EXECUTABLE)  $(CPP_INCLUDES) $(CPP_FILES) $(shell python3-config --embed --ldflags) -fPIE

all:
	$(BASE_COMPILE)

coverage:
	$(BASE_COMPILE) --coverage

COVERAGE_FILE = coverage.info

coverage-html:
	lcov --no-external --directory . --capture -o $(COVERAGE_FILE) && \
	lcov --remove $(COVERAGE_FILE) "*CAN-Decoder/*" -o $(COVERAGE_FILE) && \
	genhtml --demangle-cpp -o coverage $(COVERAGE_FILE)

clean:
	rm -rf \
		*.gcda \
		*.gcov \
		*.gcno \
		$(COVERAGE_FILE) \
		coverage \
		$(EXECUTABLE)
