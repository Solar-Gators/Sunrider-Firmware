#include "inc/DataLinkRecive.hpp"
#include "Logger.hpp"

void DataLinkReceive::init() {
    this->flush();
}

void DataLinkReceive::flush() {
    this->arrayIndex = 0;
    this->escaped = false;
    this->in_progress = false;
}

bool DataLinkReceive::read(u_int8_t data) {
    if (this->escaped) {
        this->escaped = false;
    }

    else if (data == ESCAPE) {
        this->escaped = true;
        return false;
    }

    else if (data == START) {
        if (this->in_progress) {
            this->flush();
            Logger::info("Error: There was multiple start conditions\n");
            return false;
        }
        this->in_progress = true;
        return false;
    }

    else if (data == END) {
        if (!this->in_progress) {
            this->flush();
            Logger::info("Error: There was an end condition without a start\n");
            return false;
        }
        this->in_progress = false;
        return true;
    }

    if (!this->in_progress) {
        this->flush();
        Logger::info("Error: There must be a start condition before a transmission can occur\n");
        return false;
    }

    this->buffer[this->arrayIndex] = data;
    this->arrayIndex++;
    return false;
}
