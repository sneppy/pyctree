#include "pyctree_tree.h"

/* The methods of the Tree type. */
static PyMethodDef Tree_Methods[] = {
	DEFINE_PY_METHOD(Tree, copy, PyCFunction, METH_NOARGS, NULL),
	DEFINE_PY_METHOD(Tree, get, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(Tree, find, PyCFunction, METH_FASTCALL, NULL), // Deprecated
	DEFINE_PY_METHOD(Tree, add, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(Tree, update, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(Tree, remove, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(Tree, discard, PyCFunction, METH_FASTCALL, NULL),
	DEFINE_PY_METHOD(Tree, clear, PyCFunction, METH_NOARGS, NULL)
};

/* Definition of the Python sequence API for Tree. */
static PySequenceMethods Tree_SequenceMethods = {
	.sq_length   = (lenfunc)Tree_len,
	.sq_contains = (objobjproc)Tree_contains,
};

PyTypeObject Tree_T = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name      = "pyctree.Tree",
	.tp_doc       = NULL,
	.tp_basicsize = sizeof(Tree),
	.tp_itemsize  = 0,
	.tp_flags     = Py_TPFLAGS_DEFAULT,

	.tp_new     = PyType_GenericNew,
	.tp_init    = (initproc)Tree_init,
	.tp_dealloc = (destructor)Tree_dealloc,
	.tp_str     = (reprfunc)Tree_str,

	.tp_members = NULL,
	.tp_methods = Tree_Methods,

	.tp_as_sequence = &Tree_SequenceMethods,
	.tp_as_mapping  = NULL,

	.tp_iter     = (getiterfunc)Tree_iter,
	.tp_iternext = NULL
};

PyTypeObject TreeIterator_T = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name      = "pyctree.TreeIterator",
	.tp_doc       = NULL,
	.tp_basicsize = sizeof(TreeIterator),
	.tp_itemsize  = 0,
	.tp_flags     = Py_TPFLAGS_DEFAULT,

	.tp_new     = PyType_GenericNew,
	.tp_init    = NULL, // Not callable from Python
	.tp_dealloc = (destructor)TreeIterator_dealloc,

	.tp_iter     = PyObject_SelfIter,
	.tp_iternext = (iternextfunc)TreeIterator_next
};

static void Tree_Impl_build_str(binary_node_t* node, size_t depth, PyObject** repr)
{
	// Generate tree prefix
	PyObject* vline = PyUnicode_FromString("| ");
	PyObject* prefix = PySequence_Repeat(vline, depth);

	// Concat object string
	*repr = PyUnicode_FromFormat("%U\n%U|>%R", *repr, prefix, node->item);

	Py_DECREF(prefix);
	Py_DECREF(vline);
}

/* Helper function to insert a new item in the
   tree, update the root of the tree and update
   the number of nodes. */
inline int Tree_Impl_insert(Tree* tree, PyObject* item)
{
	assert(item != NULL);

	// Insert item, also acquires ref
	binary_node_t* new_root = tree_insert_item(tree->root, item);
	if (!new_root)
	{
		// TODO: Handle error
		return -1;
	}

	tree->root = new_root;
	tree->num_nodes++;

	return 0;
}

/* Helper function to remove the first item that
   matches the key from the tree. It destroys the
   evicted node and udpates the root of the tree and
   the number of nodes. */
inline int Tree_Impl_remove(Tree* tree, binary_node_t* node)
{
	// Remove from tree
	binary_node_t* evicted = node;
	binary_node_t* new_root = tree_remove(&evicted);
	if (!evicted)
	{
		// TODO: Handle error
		return -1;
	}

	// Destroy evicted node, also releases ref
	binary_node_destroy(evicted);

	tree->root = new_root;
	tree->num_nodes--;

	return 0;
}

int Tree_init(Tree* self, PyObject* args)
{
	PyObject* init_list = NULL;
	if (!PyArg_ParseTuple(args, "|O", &init_list))
	{
		// TODO: Better error
		return -1;
	}

	// Destroy existing tree
	if (self->root)
	{
		tree_reset(self->root);
	}

	// Init tree
	self->root = NULL;
	self->num_nodes = 0;

	if (init_list)
	{
		// TODO: Support another tree as input
		PyObject* it = PyObject_GetIter(init_list);
		if (!it)
		{
			// TODO: Better error
			PyErr_SetString(PyExc_TypeError,
			                "The input must be an iterable object");
			return -1;
		}

		// TODO: Initialize set with items from given iterable
		PyObject* item = NULL;
		while ((item = PyIter_Next(it)))
		{
			Tree_Impl_insert(self, item);
			Py_DECREF(item);
		}

		if (PyErr_Occurred())
		{
			// Propagate error
			return -1;
		}

		Py_DECREF(it);
	}

	return 0;
}

void Tree_dealloc(Tree* self)
{
	// Remove all nodes
	Tree_clear(self);
}

Py_ssize_t Tree_len(Tree* self)
{
	return self->num_nodes;
}

int Tree_contains(Tree* self, PyObject* key)
{
	return tree_find(self->root, key) != NULL;
}

PyObject* Tree_str(Tree* self)
{
	// Buidl representation string
	PyObject* repr_tmp = PyUnicode_FromStringAndSize(NULL, 0);
	tree_visit(self->root, (tree_visit_cb_t)Tree_Impl_build_str, (void*)&repr_tmp);
	PyObject* repr = PyUnicode_FromFormat("{%U\n}", repr_tmp);
	Py_DECREF(repr_tmp);

	return repr;
}

Tree* Tree_copy(Tree* self)
{
	// Spawn a new tree
	Tree* new_tree = PyObject_New(Tree, &Tree_T);

	// Clone tree structure
	binary_node_t* new_tree_root = NULL;
	if (self->root)
	{
		new_tree_root = tree_clone_subtree(self->root);
	}

	new_tree->root = new_tree_root;
	new_tree->num_nodes = self->num_nodes;
	assert(new_tree->num_nodes == tree_size(new_tree->root));

	return new_tree;
}

PyObject* Tree_get(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (num_args < 1)
	{
		INVALID_NUM_ARGS_AT_LEAST(get, 1, num_args);
		return NULL;
	}
	if (num_args > 2)
	{
		INVALID_NUM_ARGS_AT_MOST(get, 2, num_args);
		return NULL;
	}

	// Find node using key
	binary_node_t* node = tree_find(self->root, args[0]);
	if (node)
	{
		// Return item found
		RETURN_NEW_REF(node->item);
	}

	if (num_args == 2)
	{
		// Return provided default value
		RETURN_NEW_REF(args[1]);
	}

	// Return None object
	RETURN_NONE
}

PyObject* Tree_find(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (DEPRECATED_METHOD_ALT(find, get) < 0)
	{
		// Warning treated as exception
		return NULL;
	}

	if (num_args != 1)
	{
		INVALID_NUM_ARGS_ONE(find, num_args);
		return NULL;
	}

	// Find node using key
	binary_node_t* node = tree_find(self->root, args[0]);
	if (node)
	{
		// Return item found
		RETURN_NEW_REF(node->item);
	}

	// Return None object
	RETURN_NONE
}

PyObject* Tree_add(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (num_args != 1)
	{
		INVALID_NUM_ARGS_ONE(find, num_args);
		return NULL;
	}

	// Insert item in tree
	Tree_Impl_insert(self, args[0]);

	RETURN_NONE
}

PyObject* Tree_update(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	for (Py_ssize_t idx = 0; idx < num_args; ++idx)
	{
		PyObject* it = PyObject_GetIter(args[idx]);
		if (!it)
		{
			// Expect an iterable
			PyErr_Format(PyExc_TypeError, "'%s' object is not iterable", Py_TYPE(args[idx])->tp_name);
			return NULL;
		}

		Py_DECREF(it);
	}

	for (Py_ssize_t idx = 0; idx < num_args; ++idx)
	{
		PyObject* it = PyObject_GetIter(args[idx]);
		PyObject* item = NULL;

		while ((item = PyIter_Next(it)))
		{
			// Insert item in tree
			Tree_Impl_insert(self, item);
			Py_DECREF(item);
		}

		if (PyErr_Occurred())
		{
			// Propagate error
			return NULL;
		}

		Py_DECREF(it);
	}

	RETURN_NONE
}

PyObject* Tree_remove(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (num_args != 1)
	{
		INVALID_NUM_ARGS_ONE(remove, num_args);
		return NULL;
	}

	// Find node to remove
	binary_node_t* node = tree_find(self->root, args[0]);
	if (!node)
	{
		// Raise key error
		PyErr_SetObject(PyExc_KeyError, args[0]);
		return NULL;
	}

	if (Tree_Impl_remove(self, node) < 0)
	{
		// Some error occured
		return NULL;
	}

	RETURN_NONE
}

PyObject* Tree_discard(Tree* self, PyObject* const* args, Py_ssize_t num_args)
{
	if (num_args != 1)
	{
		INVALID_NUM_ARGS_ONE(discard, num_args);
		return NULL;
	}

	// Find node to remove
	binary_node_t* node = tree_find(self->root, args[0]);
	if (node && Tree_Impl_remove(self, node) < 0)
	{
		// Some error occured
		return NULL;
	}

	RETURN_NONE
}

PyObject* Tree_clear(Tree* self)
{
	// Reset tree to initial state
	if (self->root)
	{
		tree_reset(self->root);
		self->root = NULL;
		self->num_nodes = 0;
	}

	RETURN_NONE
}

TreeIterator* Tree_iter(Tree* self)
{
	// Create iterator starting from min node
	TreeIterator* it = PyObject_New(TreeIterator, &TreeIterator_T);
	it->node = self->root ? tree_min(self->root) : NULL;
	it->owner = self;
	Py_INCREF(self); // Keep alive as long as iterator is alive

	return it;
}

void TreeIterator_dealloc(TreeIterator* self)
{
	// Release tree if not needed anymore by iterator
	Py_DECREF(self->owner);
}

PyObject* TreeIterator_next(TreeIterator* self)
{
	if (!self->node)
	{
		// Stop iteration
		PyErr_SetNone(PyExc_StopIteration);
		return NULL;
	}

	// Get item and increment iterator
	PyObject* item = self->node->item;
	self->node = self->node->next;

	RETURN_NEW_REF(item);
}
