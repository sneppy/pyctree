#pragma once

#include "pyctree_tree.h"

/* Python type used to implement a binary tree. */
typedef union
{
	/* Base type. */
	Tree super;

	struct
	{
		PyObject_HEAD

		/* The root node. */
		binary_node_t* root;

		/* The number of items in the set. */
		size_t num_items;
	};
} SortedSet;

/* The sorted set python type object. */
extern PyTypeObject SortedSet_T;

/* Initialize the sorted set. */
int SortedSet_init(SortedSet* self, PyObject* args);

/* Insert an item in the set. If a collision
   occurs, the item is not inserted. */
PyObject* SortedSet_add(SortedSet* self, PyObject* const* args, Py_ssize_t num_args);

/* Insert items from zero or more iterable
   objects. If the iterables contain
   multiple items with the same key, only
   the first encountered item will be
   inserted. */
PyObject* SortedSet_update(SortedSet* self, PyObject* const* args, Py_ssize_t num_args);
