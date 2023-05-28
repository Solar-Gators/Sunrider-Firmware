#include "inc/NetworkReceive.hpp"

void NetworkReceive::fromByteArray(u_int8_t *buff) {
    this->can_id = (int)buff[3] | ((int)buff[2] << 8) | ((int)buff[1] << 16) | ((int)buff[0] << 24);
    this->instance_id = buff[4];
    this->size = buff[5];
    this->data = &buff[6];
}
