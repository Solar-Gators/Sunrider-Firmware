#include <Python.h>
class PythonHttp {
    private:
        PyObject *parameters;
        void addParameter(const char* name, PyObject* value);
    public:
        void init();
        void addData(const char* name, uint32_t data);
        void addData(const char* name, uint16_t data);
        void addData(const char* name, int16_t data);
        void addData(const char* name, uint8_t data);
        void addData(const char* name, bool data);
        void addData(const char* name, float data);
        PyObject* getParameters();
        void flush();
};
