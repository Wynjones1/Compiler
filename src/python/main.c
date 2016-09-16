#pragma push_macro("_DEBUG")
#ifdef _DEBUG
#undef _DEBUG
#endif
#include "Python.h"
#pragma pop_macro("_DEBUG")

static PyObject *
spam_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return Py_BuildValue("i", sts);
}