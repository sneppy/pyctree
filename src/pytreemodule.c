#include "pytreemodule.h"

/* @brief Called to initialize the Python module,
   upon importing it in Python. */
PyMODINIT_FUNC PyInit_pytree()
{
	int status = 0;

	// Test all types are ready
	for (int idx = 0; idx < ARRAY_COUNT(pytreetypes); ++idx)
	{
		status = PyType_Ready(pytreetypes[idx].type);
		if (status < 0)
		{
			// TODO: Print error
			return NULL;
		}
	}

	// Create the Python module
	PyObject* module = PyModule_Create(&pytreemodule);
	if (!module)
	{
		// Failed to create module
		return NULL;
	}

	for (int idx = 0; idx < ARRAY_COUNT(pytreetypes); ++idx)
	{
		status = PyModule_AddObject(module,
		                            pytreetypes[idx].name,
									(PyObject*)pytreetypes[idx].type);
		if (status < 0)
		{
			Py_DECREF(module);
			return NULL;
		}
	}

	return module;
}
