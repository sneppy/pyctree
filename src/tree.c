#include "tree.h"

/* Initialize the fields of a binary tree. */
inline void binary_node_init(struct binary_node* node)
{
	node->parent = NULL;
	node->left = node->right = NULL;
	node->next = node->prev = NULL;
	node->color = BINARY_NODE_COLOR_RED;
}

/* Returns true if node is not NULL and is red. */
inline int binary_node_red(struct binary_node* node)
{
	return node && node->color == BINARY_NODE_COLOR_RED;
}

/* Returns true if node is NULL or is black. */
inline int binary_node_black(struct binary_node* node)
{
	return !binary_node_red(node);
}

/* Swap the value of two nodes. */
inline void swap_nodes(struct binary_node* lhs, struct binary_node* rhs)
{
	// Refs do not need to be incremented/decremented
	PyObject* tmp = lhs->item;
	lhs->item = rhs->item;
	rhs->item = tmp;
}

/* Insert node as left child of another node. */
inline void binary_node_insert_left(struct binary_node* parent, struct binary_node* node)
{
	assert(node != NULL);
	assert(parent != NULL);
	assert(parent->left == NULL);

	struct binary_node* prev = parent->prev;

	parent->left = node;
	parent->prev = node;
	
	node->parent = parent;
	node->next = parent;
	node->prev = prev;

	if (prev)
	{
		prev->next = node;
	}
}

/* Insert node as right child of another node. */
inline void binary_node_insert_right(struct binary_node* parent, struct binary_node* node)
{
	assert(node != NULL);
	assert(parent != NULL);
	assert(parent->right == NULL);

	struct binary_node* next = parent->next;

	parent->right = node;
	parent->next = node;
	
	node->parent = parent;
	node->prev = parent;
	node->next = next;

	if (next)
	{
		next->prev = node;
	}
}

/* Remove a node from the tree structure. It
   assumes that the node as at most a right
   child, but never has a left child.
   
   Returns a pointer to the node used to replace
   the evicted node, which can be NULL if the
   node is a leaf node. */
inline struct binary_node* tree_evict_node(struct binary_node* node)
{
	// Make sure node is semi-leaf
	assert(node != NULL);
	assert(node->left == NULL);

	struct binary_node* parent = node->parent;
	struct binary_node* repl = NULL;
	
	if ((repl = node->right))
	{
		repl->parent = parent;
		repl->prev = node->prev;
		if (repl->prev)
			repl->prev->next = repl;
	}
	else
	{
		if (node->prev)
			node->prev->next = node->next;
		
		if (node->next)
			node->next->prev = node->prev;
	}

	if (parent)
	{
		if (parent->left == node)
			parent->left = repl;
		else // if (parent->right == node)
			parent->right = repl;
	}

	return repl;
}

/* Called to repair the RB tree structure after
   node insertion. Takes a pointer to the
   inserted node. */
static void tree_repair(struct binary_node* node)
{
	//
}

inline struct binary_node* binary_node_create(PyObject* item)
{
	assert(item != NULL);

	// Alloc memory for new node
	size_t node_size = sizeof(struct binary_node);
	struct binary_node* new_node = PyMem_Malloc(node_size);

	// Init node
	binary_node_init(new_node);

	// Set item, increment ref count
	new_node->item = item;
	Py_INCREF(item);

	return new_node;
}

inline void binary_node_destroy(struct binary_node* node)
{
	assert(node != NULL);

	// Decrement item ref count
	Py_XDECREF(node->item);
	node->item = NULL;

	// Destroy node
	PyMem_Free(node);
}

struct binary_node* tree_bisect_right(struct binary_node* root, PyObject* key)
{
	struct binary_node* it = root;
	struct binary_node* p = NULL;
	while (it)
	{
		// Set parent
		p = it;

		// TODO: Handle errors
		if (PyObject_RichCompareBool(key, it->item, Py_LT))
		{
			it = it->left;
		}
		else
		{
			it = it->right;
		}
	}

	// Return last visited node
	return p;
}

struct binary_node* tree_find(struct binary_node* root, PyObject* key)
{
	for (struct binary_node* it = root; it;)
	{
		// TODO: Handle errors
		if (PyObject_RichCompareBool(key, it->item, Py_LT))
		{
			it = it->left;
		}
		else if (PyObject_RichCompareBool(key, it->item, Py_GT))
		{
			it = it->right;
		}
		else
		{
			return it;
		}
	}

	return NULL;
}

struct binary_node* tree_insert(struct binary_node* root, struct binary_node* node)
{
	struct binary_node* parent = tree_bisect_right(root, node->item);
	if (parent)
	{
		if (PyObject_RichCompareBool(node->item, parent->item, Py_LT))
		{
			binary_node_insert_left(parent, node);
		}
		else
		{
			binary_node_insert_right(parent, node);
		}
	}

	// Repair tree after insertion
	tree_repair(node);

	// Return new root
	return tree_root(node);
}

struct binary_node* tree_insert_item(struct binary_node* root, PyObject* item)
{
	assert(item != NULL);

	// Create new node
	struct binary_node* node = binary_node_create(item);

	// Insert node in tree and return new root
	return tree_insert(root, node);
}

struct binary_node* tree_remove(struct binary_node** node)
{
	assert(node != NULL && *node != NULL);

	struct binary_node* parent = (*node)->parent;
	struct binary_node* next = (*node)->next;

	// If node has both children
	if ((*node)->left && (*node)->right)
	{
		// Swap with next
		swap_nodes(*node, next);
		next = *node;
		*node = (*node)->next; // We will evict this node
	}

	// Evict node from tree
	struct binary_node* repl = tree_evict_node(*node);

	if (binary_node_black(*node))
	{
		// Repair tree if evicted node is black
		// TODO
	}

	// Return new root
	if (repl)
	{
		return tree_root(parent);
	}
	else if (parent)
	{
		return tree_root(next);
	}
	
	// Removed last node of the tree
	return NULL;
}

void tree_reset(struct binary_node* root)
{
	// We can simply iterate over the tree from
	// left to right
	struct binary_node* it = tree_min(root);
	struct binary_node* next = NULL;

	for (; it; it = next)
	{
		next = it->next;
		binary_node_destroy(it);
	}
}
