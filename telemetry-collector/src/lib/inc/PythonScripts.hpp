#include <Python.h>

#ifndef PythonScripts_H
#define PythonScripts_H
class PythonScripts {
    private:
        PyObject *uartModule;
        PyObject *httpModule;
    public:
        int init();
        PyObject* loadModule(const char *name);

        int readUART();

        void send(const char* path, PyObject *parameters);
};
#endif
