API reference
=============

[TOC]

Tree
----

The `Tree` type implements a binary tree with auto-balancing logic (red-black tree algorithm).

All methods of `Tree`, unless stated otherwise, use `__lt__` and `__gt__` operators to determine the position of a value in the tree:

```python
if lhs < rhs:
	# go left
elif lhs > rhs:
	# go right
else:
	# value found
```

The term _key_ is broadly used to refer to the property(s) of the item that determines its position in the tree.

### `#!python class Tree([iterable])`

Returns a new tree instance. The items of the tree are taken from the `iterable` object, if given.

### `#!python len(t)`

Returns the number of items in the tree.

### `#!python x in t`

Returns `True` if the tree contains one or more items matching the given key, `False` otherwise.

### `#!python add(item)`

Add `item` to the tree.

### `#!python update(*iterables)`

Insert items in the tree taken from zero or more `iterables`.

### `#!python remove(key)`

Remove the first item that matches the given key from the tree. If no item matches the key, raises a `KeyError`.

### `#!python discard(key)`

Like `remove()`, but does not raise a `KeyError` if no item matches the key.

### `#!python clear()`

Removes all items from the tree.

### `#!python copy()`

Creates a shallow copy of the tree.

### `#!python left_bound(key)`

Returns the item that partitions the tree in such a way that all previous items are smaller and all next items are
larger or equal. The item, if it exists, is unique (though it may depend on the order in which the items were inserted
in the tree, if there are multiple items with the same key).

### `#!python right_bound(key)`

Returns the item that partitions the tree in such a way that all previous items are smaller or equal and all next items
are larger. See `#!python left_bound(key)` for additional info.

SortedSet
---------

WIP

SortedDict
----------

TODO
