#include <stdlib.h>
#define START 0xFF
#define END 0x3F
#define ESCAPE 0x2F
#define MAX_BUFFER_SIZE 5000

class DataLinkReceive {
    private:
        bool escaped;
        bool in_progress;

    public:
        u_int8_t buffer[MAX_BUFFER_SIZE];
        int arrayIndex;
        void init();
        void flush();

        bool read(u_int8_t data);
};
