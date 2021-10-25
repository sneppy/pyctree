#pragma once

#include "python.h"

/* Color of a RB tree node. */
enum binary_node_color
{
	BINARY_NODE_COLOR_BLACK,
	BINARY_NODE_COLOR_RED
};

/* Basic implementation of a binary node type
   which contains a Python object. The node
   also has pointers to the previous and next
   nodes in sorting order. */
typedef struct binary_node
{
	/* Ptr to the item inside the node. */
	PyObject* item;

	/* Ptr to parent node. */
	struct binary_node* parent;

	/* Ptr to left child. */
	struct binary_node* left;

	/* Ptr to right child. */
	struct binary_node* right;

	/* Ptr to the next child in the sequence. */
	struct binary_node* next;

	/* Ptr to the previous child in the sequence. */
	struct binary_node* prev;

	/* Color of the node. */
	enum binary_node_color color;
} binary_node_t;

/* Type of the tree visit callback. */
typedef void(*tree_visit_cb_t)(binary_node_t*, size_t, void*);

/* Type of the tree read-only visit callback. */
typedef void(*tree_const_visit_t)(binary_node_t const*, size_t, void*);
