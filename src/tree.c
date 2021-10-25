#include "tree.h"

#define INV(dir) (1 - dir)

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

/* Returns an array with pointers to the left
   and right children. */
inline struct binary_node** binary_node_children(struct binary_node* node)
{
	assert(&node->left + 1 == &node->right);
	return &node->left;
}

/* Swap the value of two nodes. */
inline void binary_node_swap(struct binary_node* lhs, struct binary_node* rhs)
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

/* Rotate the subtree around the pivot node in
   the given direction (0 = left, 1 = right). */
inline void binary_node_rotate_dir(struct binary_node* node, int dir)
{
	assert(dir == 0 || dir == 1);

	struct binary_node* parent = node->parent;
	struct binary_node* pivot = binary_node_children(node)[INV(dir)];
	struct binary_node* child = binary_node_children(pivot)[dir];

	node->parent = pivot;
	binary_node_children(node)[INV(dir)] = child;

	pivot->parent = parent;
	binary_node_children(pivot)[dir] = node;

	if (parent)
	{
		binary_node_children(parent)[parent->right == node] = pivot;
	}

	if (child)
	{
		child->parent = node;
	}
}

/* Sets a subtree as the left child of a
   parent node. */
inline void tree_set_left_subtree(struct binary_node* parent, struct binary_node* left)
{
	assert(parent != NULL);
	assert(left != NULL);

	// The prev node is the rightmost node of the
	// subtree
	struct binary_node* prev = tree_max(left);

	parent->left = left;
	parent->prev = prev;
	left->parent = parent;
	prev->next = parent; // Prev is always non-NULL
}

/* Sets a subtree as the right child of a
   parent node. */
inline void tree_set_right_subtree(struct binary_node* parent, struct binary_node* right)
{
	assert(parent != NULL);
	assert(right != NULL);

	// The next node is the rightmost node of the
	// subtree
	struct binary_node* next = tree_min(right);

	parent->right = right;
	parent->next = next;
	right->parent = parent;
	next->prev = parent; // Next is always non-NULL
}

/* Remove a node from the tree structure.

   Returns a pointer to the node used to replace
   the evicted node, which can be NULL if the
   node is a leaf node. */
inline struct binary_node* tree_evict_node(struct binary_node* node)
{
	// Make sure node is semi-leaf
	assert(node != NULL);
	assert(node->left == NULL || node->right == NULL);

	struct binary_node* parent = node->parent;
	struct binary_node* repl = NULL;

	if ((repl = node->left) || (repl = node->right))
	{
		// We have a replacement node
		repl->parent = parent;
	}

	if (parent)
	{
		if (parent->left == node)
			parent->left = repl;
		else // if (parent->right == node)
			parent->right = repl;
	}

	if (node->prev)
	{
		node->prev->next = node->next;
	}

	if (node->next)
	{
		node->next->prev = node->prev;
	}

	return repl;
}

/* Called to repair the RB tree structure after
   node insertion. Takes a pointer to the
   inserted node. */
static void tree_repair(struct binary_node* node)
{
	assert(node != NULL);
	assert(node->color = BINARY_NODE_COLOR_RED);

	for (;;)
	{
		struct binary_node* parent = node->parent;

		if (!parent)
		{
			// Node is root, make black
			node->color = BINARY_NODE_COLOR_BLACK;
			return;
		}
		else if (binary_node_black(parent))
		{
			// Leave red
			return;
		}
		else
		{
			assert(parent->parent != NULL); // Grand cannot be NULL
			struct binary_node* grand = parent->parent;
			struct binary_node* uncle = grand->left != parent
									  ? grand->left
									  : grand->right;

			if (binary_node_red(uncle))
			{
				// We can make both parent and uncle black
				// and repair grand
				uncle->color = parent->color = BINARY_NODE_COLOR_BLACK;
				grand->color = BINARY_NODE_COLOR_RED;
				node = grand;
			}
			else // Uncle is black or NULL
			{
				int dir = grand->right == parent;

				if (binary_node_children(parent)[dir] != node)
				{
					// Rotate to the outside and recolor
					binary_node_rotate_dir(parent, dir);
					parent = node;
				}

				// Rotate grand
				binary_node_rotate_dir(grand, INV(dir));
				parent->color = BINARY_NODE_COLOR_BLACK;
				grand->color = BINARY_NODE_COLOR_RED;
				return;
			}
		}
	}
}

/* Called after node removal to repair the RB
   tree structure. It takes a pointer to the
   node that replaced the evicted node and a
   pointer to the parent (in case repl is
   NULL). */
static void tree_repair_removed(struct binary_node* repl, struct binary_node* parent)
{
	if (!repl && !parent)
		return; // Nothing to do

	if (binary_node_red(repl) || !parent)
	{
		// Make node black to rebalance
		repl->color = BINARY_NODE_COLOR_BLACK;
		return;
	}

	do
	{
		int dir = parent->right == repl;
		struct binary_node* sibling = binary_node_children(parent)[INV(dir)];

		if (binary_node_red(sibling))
		{
			binary_node_rotate_dir(parent, dir);
			sibling->color = parent->color;
			parent->color = BINARY_NODE_COLOR_RED;
			sibling = binary_node_children(parent)[INV(dir)];
		}

		// Now sibling is surely black
		assert(sibling && sibling->color == BINARY_NODE_COLOR_BLACK);
		if (binary_node_black(sibling->left) && binary_node_black(sibling->right))
		{
			sibling->color = BINARY_NODE_COLOR_RED;
			if (binary_node_red(parent))
			{
				parent->color = BINARY_NODE_COLOR_BLACK;
				return; // Repair complete
			}

			// Up one level
			repl = parent;
		}
		else
		{
			// At least one of the children is red
			struct binary_node* close = binary_node_children(sibling)[dir];
			struct binary_node* distant = binary_node_children(sibling)[INV(dir)];

			if (binary_node_red(close))
			{
				binary_node_rotate_dir(sibling, INV(dir));
				close->color = sibling->color;
				sibling->color = BINARY_NODE_COLOR_RED;
				distant = sibling;
				sibling = close;
			}

			binary_node_rotate_dir(parent, dir);
			sibling->color = parent->color;
			parent->color = BINARY_NODE_COLOR_BLACK;
			distant->color = BINARY_NODE_COLOR_BLACK;
			return; // Repair completed
		}
	} while ((parent = repl->parent));
}

static void tree_visit_df_impl(struct binary_node* root, size_t depth, tree_visit_cb_t visit_cb, void* payload)
{
	visit_cb(root, depth, payload);

	if (root->left)
		tree_visit_df_impl(root->left, depth + 1, visit_cb, payload);

	if (root->right)
		tree_visit_df_impl(root->right, depth + 1, visit_cb, payload);
}

struct binary_node* binary_node_create(PyObject* item)
{
	assert(item != NULL);

	// Alloc memory for new node
	size_t node_size = sizeof(struct binary_node);
	struct binary_node* new_node = PyMem_Malloc(node_size);

	// Init node
	binary_node_init(new_node);

	// Set item
	Py_INCREF(item);
	new_node->item = item;

	return new_node;
}

void binary_node_destroy(struct binary_node* node)
{
	assert(node != NULL);

	// Release Python item
	Py_DECREF(node->item);
	node->item = NULL;

	// Destroy node
	PyMem_Free(node);
}

size_t tree_size(struct binary_node* root)
{
   size_t size = 1;

   if (root->left)
      size += tree_size(root->left);

   if (root->right)
      size += tree_size(root->right);

   return size;
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

	// Create new node and insert in tree
	struct binary_node* node = binary_node_create(item);
	struct binary_node* new_root = tree_insert(root, node);
	if (!new_root)
	{
		// Destroy created node
		binary_node_destroy(node);
	}

	return new_root;
}

struct binary_node* tree_remove(struct binary_node** node)
{
	assert(node != NULL && *node != NULL);

	struct binary_node* next = (*node)->next;
	assert(*node && next == tree_min((*node)->left));

	// If node has both children
	if ((*node)->left && (*node)->right)
	{
		// Swap with next
		binary_node_swap(*node, next);
		*node = next; // We will evict this node
	}

	// Evict node from tree
	struct binary_node* parent = (*node)->parent;
	struct binary_node* repl = tree_evict_node(*node);

	if (binary_node_black(*node))
	{
		// Repair tree if evicted node is black
		tree_repair_removed(repl, parent);
	}

	// Return new root
	if (repl)
	{
		return tree_root(repl);
	}
	else if (parent)
	{
		return tree_root(parent);
	}

	// Removed last node of the tree
	return NULL;
}

void tree_reset(struct binary_node* root)
{
	assert(root != NULL);

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

void tree_destroy_subtree(struct binary_node* root)
{
	assert(root != NULL);

	if (root->left)
	{
		tree_destroy_subtree(root->left);
	}

	if (root->right)
	{
		tree_destroy_subtree(root->right);
	}

	// Destroy root
	binary_node_destroy(root);
}

struct binary_node* tree_clone_subtree(struct binary_node* src)
{
	assert(src != NULL);

	// Make a shallow copy of the node
	struct binary_node* dst = binary_node_create(src->item);
	dst->color = src->color;

	if (src->left)
	{
		// Clone left subtree
		struct binary_node* left = tree_clone_subtree(src->left);
		tree_set_left_subtree(dst, left);
	}

	if (src->right)
	{
		// Clone right subtree
		struct binary_node* right = tree_clone_subtree(src->right);
		tree_set_right_subtree(dst, right);
	}

	return dst;
}

struct binary_node* tree_copy_subtree(struct binary_node* dst, struct binary_node* src)
{
	assert(dst == NULL || dst->item != NULL);
	assert(src == NULL || src->item != NULL);

	if (!dst && !src)
	{
		// Nothing to do here
		return NULL;
	}
	else if (!src)
	{
		// If source node is null, we need to remove dst node
		tree_destroy_subtree(dst);
		return NULL;
	}
	else if (!dst)
	{
		// Create entire subtree
		return tree_clone_subtree(src);
	}

	// Both exists, just copy ref and color
	Py_DECREF(dst->item);
	dst->item = src->item;
	Py_INCREF(dst->item);
	dst->color = src->color;

	// Copy left subtree
	struct binary_node* left = tree_copy_subtree(dst->left, src->left);
	if (left != dst->left)
	{
		// Set new subtree
		assert(dst->left == NULL);
		tree_set_left_subtree(dst, left);
	}

	struct binary_node* right = tree_copy_subtree(dst->right, src->right);
	if (right != dst->right)
	{
		// Set new subtree
		assert(dst->right == NULL);
		tree_set_right_subtree(dst, right);
	}

	// Return existing node
	return dst;
}

void tree_visit_df(struct binary_node* root, tree_visit_cb_t visit_cb, void* payload)
{
	if (!root) return;
	tree_visit_df_impl(root, 0, visit_cb, payload);
}

void tree_visit_bf(struct binary_node* root, tree_visit_cb_t visit_cb, void* payload)
{
	// TODO
}

#undef INV
