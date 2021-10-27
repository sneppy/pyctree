#pragma once

#include "python.h"
#include "pyctree_tree.h"
#include "pyctree_sorted_set.h"

#define PYCTREE_MODULE

/* The name of the Python module. */
static char const module_name[] = "pyctree";

/* Definition for the Python module. */
static struct PyModuleDef pyctreemodule = {
	.m_base = PyModuleDef_HEAD_INIT,
	.m_name = module_name,
	.m_doc  = NULL,
	.m_size = -1
};

/* List of python types. */
static struct python_type_def pyctreetypes[] = {
	{.type = &Tree_T, .name = "Tree"},
	{.type = &SortedSet_T, .name = "SortedSet"}
};
