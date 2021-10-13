#pragma once

#include "python.h"
#include "pytree_tree.h"

#define PYTREE_MODULE

/* The name of the Python module. */
static char const module_name[] = "pytree";

/* Definition for the Python module. */
static struct PyModuleDef pytreemodule = {
	.m_base = PyModuleDef_HEAD_INIT,
	.m_name = module_name,
	.m_doc  = NULL,
	.m_size = -1
};

/* List of python types. */
static struct python_type_def pytreetypes[] = {
	{.type = &Tree_T, .name = "Tree"}
};
