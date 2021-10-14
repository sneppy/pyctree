#pragma once

#include "tree_types.h"

/* Create a new binary tree with the given
   Python item. Increases the ref count of
   Python item.
   
   Returns a pointer to the created node. */
struct binary_node* binary_node_create(PyObject* item);

/* Destroys a node of the tree and decrements
   the ref count of the Python object contained
   in the node. */
void binary_node_destroy(struct binary_node* node);

/* Returns the root of the tree the given
   node belongs to. */
inline struct binary_node* tree_root(struct binary_node* node)
{
	assert(node != NULL);
	for (; node->parent; node = node->parent);
	return node;
}

/* Returns the leftmost node of the subtree.
   It is also the node with the smallest
   value according to the comparison
   operation used. */
inline struct binary_node* tree_min(struct binary_node* root)
{
	assert(root != NULL);
	for (; root->left; root = root->left);
	return root;
}

/* Returns the rightmost node of the subtree.
   It is also the node with the largest
   value according to the comparison
   operation used. */
inline struct binary_node* tree_max(struct binary_node* root)
{
	assert(root != NULL);
	for (; root->right; root = root->right);
	return root;
}

/* Returns the number of nodes in the tree. */
size_t tree_size(struct binary_node* root);

/* Returns the last node along the path given
   by the key. When an item matches the key
   it moves to the right child. */
struct binary_node* tree_bisect_right(struct binary_node* root, PyObject* key);

/* Returns a pointer to the first node that
   matches the key, or NULL if no such node
   exists. */
struct binary_node* tree_find(struct binary_node* root, PyObject* key);

/* Insert a node in the tree at the right position
   and repairs the tree if necessary.
   
   Returns a pointer to the new root of the tree. */
struct binary_node* tree_insert(struct binary_node* root, struct binary_node* node);

/* Insert a new item in the tree. This function
   takes care of creating a new node and getting
   a reference to the python Object.

   It returns a pointer to the new root of the
   tree. */
struct binary_node* tree_insert_item(struct binary_node* root, PyObject* item);

/* Remove a node from the tree. This function only
   evicts a node from the tree, it does not dispose
   it nor does it decrement the ref count of the
   Python object.

   It returns a pointer to the new root of the tree
   and sets the first argument to point to the node
   that has actually been evicted from the tree. */
struct binary_node* tree_remove(struct binary_node** node);

/* Remove all nodes of the tree, leaving the tree
   empty. The node given must be the root of the
   tree. */
void tree_reset(struct binary_node* root);

/* Destroy all the nodes in the subtree. */
void tree_destroy_subtree(struct binary_node* root);

/* Clone subtree spawning from given node. */
struct binary_node* tree_clone_subtree(struct binary_node* src);

/* Copy the structure from the source subtree
   to the destination subtree. */
struct binary_node* tree_copy_subtree(struct binary_node* dst, struct binary_node* src);
