#include "pyctreemodule.h"

/* Called to initialize the Python module. */
PyMODINIT_FUNC PyInit_pyctree()
{
	int status = 0;

	// Test all types are ready
	for (uint32_t idx = 0; idx < ARRAY_COUNT(pyctreetypes); ++idx)
	{
		status = PyType_Ready(pyctreetypes[idx].type);
		if (status < 0)
		{
			// TODO: Print error
			return NULL;
		}
	}

	// Create the Python module
	PyObject* module = PyModule_Create(&pyctreemodule);
	if (!module)
	{
		// Failed to create module
		return NULL;
	}

	for (uint32_t idx = 0; idx < ARRAY_COUNT(pyctreetypes); ++idx)
	{
		status = PyModule_AddObject(module,
		                            pyctreetypes[idx].name,
									(PyObject*)pyctreetypes[idx].type);
		if (status < 0)
		{
			Py_DECREF(module);
			return NULL;
		}
	}

	return module;
}
