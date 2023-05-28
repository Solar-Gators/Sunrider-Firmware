#include "Logger.hpp"
#include <stdio.h>

void Logger::info(const char * str) {
    printf("%s", str);
    fflush(stdout);
}
