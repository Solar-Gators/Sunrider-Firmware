#include <stdlib.h>

class NetworkReceive {
    public:
        u_int32_t can_id;
        u_int16_t instance_id;
        u_int16_t size;
        u_int16_t crc;
        u_int8_t *data;

        void fromByteArray(u_int8_t *buff);
};
