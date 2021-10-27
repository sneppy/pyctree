#include "pyctree_sorted_set.h"

/* The methods of SortedSet type. */
static PyMethodDef SortedSet_methods[] = {
	DEFINE_PY_METHOD(SortedSet, add, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(SortedSet, update, PyCFunction, METH_FASTCALL, NULL),
	END_PY_METHOD_LIST
};

PyTypeObject SortedSet_T = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name      = "pyctree.SortedSet",
	.tp_doc       = NULL,
	.tp_basicsize = sizeof(SortedSet),
	.tp_itemsize  = 0,
	.tp_flags     = Py_TPFLAGS_DEFAULT,
	.tp_base      = &Tree_T,

	.tp_new     = PyType_GenericNew,
	.tp_init    = (initproc)SortedSet_init,
	//.tp_dealloc = (destructor)Tree_dealloc,
	//.tp_str     = (reprfunc)Tree_str,

	//.tp_members = NULL,
	.tp_methods = SortedSet_methods,

	//.tp_iter     = (getiterfunc)Tree_iter,
	//.tp_iternext = NULL
};

/* Helper function to insert a new item in the
   tree, update the root of the tree and update
   the number of nodes.

   If an item with the same key exists, the item
   is not inserted. */
inline int SortedSet_Impl_insert(SortedSet* set, PyObject* item)
{
	assert(item != NULL);

	// Insert unique item
	binary_node_t* node = binary_node_create(item);
	binary_node_t* new_node = node;
	binary_node_t* new_root = tree_insert_unique(set->root, &node);

	if (node != new_node)
	{
		// Destroy new node (also decref item)
		binary_node_destroy(new_node);
		assert(set->root == new_root);
	}
	else
	{
		// Update set
		set->root = new_root;
		set->num_items++;
	}

	return 0;
}

inline int SortedSet_Impl_update(SortedSet* set, PyObject* iterable)
{
	// Get iterator
	PyObject* it = PyObject_GetIter(iterable);
	if (!it)
	{
		// TODO: Better error
		PyErr_SetString(PyExc_TypeError,
						"The input must be an iterable object");
		return -1;
	}

	// Initialize set with items from given iterable
	PyObject* item = NULL;
	while ((item = PyIter_Next(it)))
	{
		SortedSet_Impl_insert(set, item);
		Py_DECREF(item);
	}

	Py_DECREF(it);

	return 0;
}

int SortedSet_init(SortedSet* self, PyObject* args)
{
	PyObject* init_values = NULL;
	if (!PyArg_ParseTuple(args, "|O", &init_values))
	{
		// TODO: Better error
		return -1;
	}

	// Destroy set
	if (self->root)
	{
		tree_reset(self->root);
	}

	self->root = NULL;
	self->num_items = 0;

	// Update from iterable
	if (init_values && SortedSet_Impl_update(self, init_values) < 0)
	{
		// Some error occured
		return -1;
	}

	return 0;
}

Py_ssize_t SortedSet_len(SortedSet* self)
{
	return self->num_items;
}

int SortedSet_contains(SortedSet* self, PyObject* key)
{
	return tree_find(self->root, key) != NULL;
}

PyObject* SortedSet_add(SortedSet* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (num_args != 1)
	{
		// Expect one argument
		INVALID_NUM_ARGS_ONE(add, num_args);
		return NULL;
	}

	// Insert item in set
	SortedSet_Impl_insert(self, args[0]);

	RETURN_NONE
}

PyObject* SortedSet_update(SortedSet* self, PyObject* const* args, Py_ssize_t num_args)
{
	for (Py_ssize_t idx = 0; idx < num_args; ++idx)
	{
		// Update from i-th iterable
		SortedSet_Impl_update(self, args[idx]);
	}

	RETURN_NONE
}
