#pragma once

#include "tree.h"

/* Python type used to implement a binary tree. */
typedef struct
{
	PyObject_HEAD

	/* Root node. */
	binary_node_t* root;

	/* Number of nodes */
	size_t num_nodes;
} Tree;

/* The tree python type object. */
extern PyTypeObject Tree_T;

/* The iterator type used to iterate over a binary tree. */
typedef struct
{
	PyObject_HEAD

	/* Node pointed by iterator. */
	binary_node_t* node;

	/* Tree this iterator belongs too. */
	Tree* owner;
} TreeIterator;

/* The tree iterator type object. */
extern PyTypeObject TreeIterator_T;

/* Called to initialize a binary tree. */
int Tree_init(Tree* self, PyObject* args);

/* Remove all the nodes and destroy tree. */
void Tree_dealloc(Tree* self);

/* Returns the number of items in the tree. */
Py_ssize_t Tree_len(Tree* self);

/* Returns true if the tree contains at least
   one item identified by the given key. */
int Tree_contains(Tree* self, PyObject* key);

/* Returns a string representation of the tree. */
PyObject* Tree_str(Tree* self);

/* Returns a copy of the tree. */
Tree* Tree_copy(Tree* self);

/* Returns the first item that matches the
   given key, or the default value (None by
   default). */
PyObject* Tree_get(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Returns the first item that matches the
   given key. */
PyObject* Tree_find(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Returns an item such that all previous items
   are less than the given key. */
PyObject* Tree_left_bound(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Returns an item such that all next items are
   greater than the given key. */
PyObject* Tree_right_bound(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Insert a new item in the tree. The tree may
   contain multiple items that match the same
   key. */
PyObject* Tree_add(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Insert multiple items in the tree. This
   method accepts zero or more iterable items. */
PyObject* Tree_update(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Remove the first item in the tree that matches
   the given key. If no such item exists, raises
   a KeyError. */
PyObject* Tree_remove(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Like Tree_remove, but returns None instead of
   raising a KeyError if item does not exist. */
PyObject* Tree_discard(Tree* self, PyObject* const* args, Py_ssize_t num_args);

/* Remove all items from the tree. */
PyObject* Tree_clear(Tree* self);

/* Returns an iterator to iterate over the nodes
   of the tree in a sorted manner. Note that the
   tree is naturally sorted so this costs nothing. */
TreeIterator* Tree_iter(Tree* self);

/* Deallocates the tree iterator. */
void TreeIterator_dealloc(TreeIterator* self);

/* Increments the tree iterator by one and returns
   the item it currently points to. */
PyObject* TreeIterator_next(TreeIterator* self);
