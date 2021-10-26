#pragma once

#include "tree_types.h"

/* Create a new binary tree with the given
   Python item. Also increases the ref count
   of the Python item.

   Returns a pointer to the created node. */
binary_node_t* binary_node_create(PyObject* item);

/* Destroys a node of the tree. Also decrements
   the ref count of the Python object owned by
   the node. */
void binary_node_destroy(binary_node_t* node);

/* Returns the Python repr of a binary node. */
inline PyObject* binary_node_repr(binary_node_t* node)
{
	// TODO: Enable debug repr
	return PyObject_Repr(node->item);
}

/* Print the repr of the Python object owned by
   the node. */
inline void binary_node_print(binary_node_t* node)
{
	assert(node != NULL);
	assert(node->item != NULL);
	PyObject_Print(node->item, stdout, 0);
}

/* Returns the root of the tree the given
   node belongs to. */
inline binary_node_t* tree_root(binary_node_t* node)
{
	assert(node != NULL);
	for (; node->parent; node = node->parent);
	return node;
}

/* Returns the leftmost node of the subtree.
   It is also the node with the smallest
   value according to the comparison
   operation used. */
inline binary_node_t* tree_min(binary_node_t* root)
{
	assert(root != NULL);
	for (; root->left; root = root->left);
	return root;
}

/* Returns the rightmost node of the subtree.
   It is also the node with the largest
   value according to the comparison
   operation used. */
inline binary_node_t* tree_max(binary_node_t* root)
{
	assert(root != NULL);
	for (; root->right; root = root->right);
	return root;
}

/* Returns the number of nodes in the tree. */
size_t tree_size(binary_node_t* root);

/* Returns the last node along the path given
   by the key. When an item matches the key
   it moves to the right child. */
binary_node_t* tree_bisect_right(binary_node_t* root, PyObject* key);

/* Returns a pointer to the first node that
   matches the key, or NULL if no such node
   exists. */
binary_node_t* tree_find(binary_node_t* root, PyObject* key);

/* Insert a node in the tree at the right position
   and repairs the tree if necessary.

   Returns a pointer to the new root of the tree. */
binary_node_t* tree_insert(binary_node_t* root, binary_node_t* node);

/* Insert a node in the tree. If a node with the
   same key already exists, it does not insert
   the node instead.

   Returns the new root of the tree and the
   inserted node or the existing node. */
binary_node_t* tree_insert_unique(binary_node_t* root, binary_node_t** node);

/* Insert a node in the tree. If a node with the
   same key already exists, it replaces it with
   the new node.

   Returns the new root of the tree and the node
   that was replaced, NULL if node was not
   replaced. */
binary_node_t* tree_insert_replace(binary_node_t* root, binary_node_t** node);

/* Insert a new item in the tree. This function
   takes care of creating a new node and getting
   a reference to the python Object.

   It returns a pointer to the new root of the
   tree. */
binary_node_t* tree_insert_item(binary_node_t* root, PyObject* item);

/* Remove a node from the tree. This function only
   evicts a node from the tree, it does not dispose
   it nor does it decrement the ref count of the
   Python object.

   It returns a pointer to the new root of the tree
   and sets the first argument to point to the node
   that has actually been evicted from the tree. */
binary_node_t* tree_remove(binary_node_t** node);

/* Remove all nodes of the tree, leaving the tree
   empty. The node given must be the root of the
   tree. */
void tree_reset(binary_node_t* root);

/* Destroy all the nodes in the subtree. */
void tree_destroy_subtree(binary_node_t* root);

/* Clone subtree spawning from given node. */
binary_node_t* tree_clone_subtree(binary_node_t* src);

/* Copy the structure from the source subtree
   to the destination subtree. */
binary_node_t* tree_copy_subtree(binary_node_t* dst, binary_node_t* src);

/* Call the visit callback with all the nodes
   in the tree. The visit is DF. Root may be
   NULL. */
void tree_visit_df(binary_node_t* root, tree_visit_cb_t visit_cb, void* payload);

/* Call the visit callback with all the nodes
   in the tree. The visit is DF. Root may be
   NULL. */
void tree_visit_bf(binary_node_t* root, tree_visit_cb_t visit_cb, void* payload);

/* Same as tree_visit_df. */
inline void tree_visit(binary_node_t* root, tree_visit_cb_t visit_cb, void* payload)
{
	tree_visit_df(root, visit_cb, payload);
}
