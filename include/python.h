#pragma once

#define PY_SSIZE_T_CLEAN

#include <assert.h>
#include <Python.h>

/* Returns the length of a static array. */
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(*x))

/* Helper macro to define a class method. */
#define DEFINE_PY_METHOD(owner, method, type, flags, doc) {\
	.ml_name  = #method,\
	.ml_meth  = (type)owner##_##method,\
	.ml_flags = flags,\
	.ml_doc   = doc\
}

/* Set TypeError for invalid number of arguments */
#define INVALID_NUM_ARGS_ONE(func, num_args) PyErr_Format(PyExc_TypeError,\
                                                          #func "() takes exactly one argument (%zu given)",\
														  num_args)

/* Helper struct to register a new Python type. */
struct python_type_def
{
	/* The object spec. */
	PyTypeObject* type;

	/* The name of the type. */
	char const* name;
};
