#include "inc/PythonHttp.hpp"

//public methods
void PythonHttp::init() {
    parameters = PyDict_New();
}

void PythonHttp::addData(const char* name, int16_t data) {
    addParameter(
        name,
        PyLong_FromLong(data)
    );
}

void PythonHttp::addData(const char* name, uint32_t data) {
    addParameter(
        name,
        PyLong_FromLong(data)
    );
}

void PythonHttp::addData(const char* name, uint16_t data) {
    addParameter(
        name,
        PyLong_FromLong(data)
    );
}

void PythonHttp::addData(const char* name, uint8_t data) {
    addParameter(
        name,
        PyLong_FromLong(data)
    );
}

void PythonHttp::addData(const char* name, float data) {
    addParameter(
        name,
        PyFloat_FromDouble(data)
    );
}

void PythonHttp::addData(const char* name, bool data) {
    PyObject *value;
    if (data) {
        value = Py_True;
    }
    else {
        value = Py_False;
    }

    addParameter(name, value);
}

PyObject * PythonHttp::getParameters() {
    return parameters;
}

void PythonHttp::flush() {
    Py_DECREF(parameters);
}

//private methods
void PythonHttp::addParameter(const char* name, PyObject* value) {
    PyDict_SetItemString(parameters, name, value);
}
