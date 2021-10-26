# PyCTree

[![Unit Test](https://github.com/sneppy/pyctree/actions/workflows/unit-test.yml/badge.svg)](https://github.com/sneppy/pyctree/actions/workflows/unit-test.yml)

PyCTree is a collection of tree-based containers for Python, written in C.

## Installation

You can download and install the latest version from PyPI:

```console
$ pip install pyctree
```

> The package is being tested on latest Py3.6, Py3.7 and Py3.8. Unit tests fail on Py3.9 and it hasn't been tested on other distributions.

### Building from source

Alternatively, you can clone this repository and manually build the module:

```console
$ git clone https://github.com/sneppy/pyctree.git
$ cd pyctree
$ python setup.py install
```

## Usage

PyCTree types are designed to be drop-in replacements for standard Python types; most methods have the same name and signature:

```python
from pyctree import Tree
from random import randint

t = Tree(range(10)) # Initialize by reading from any iterable
print(len(t)) # Get number of nodes
for x in t:
	print(x) # 1, 2, 3, 4, 5, 6, ...
	assert x in t # True

t.get(1) # Get value from tree
t.get(10, 0) # Return default value or None if key doesn't exist
t.add(randint(0, 9)) # Add a single value
t.update(range(10, 20), [100, 101]) # Add multiple values
t.remove(4) # Remove 4
try:
	t.remove(102) # KeyError
except KeyError:
	pass
t.discard(102) # Ignore missing key
u = t.copy() # Create a shallow copy of the tree
t.clear() # Remove all nodes
print([*u]) # Unpack tree items
```

Available types:

- `pyctree.Tree` a self-balancing binary tree implementation (also works as a multi set/dict as it allows duplicate keys)
- `pyctree.SortedSet` a tree-based set with sorted items (TODO)
- `pyctree.SortedDict` a tree-based dict with sorted items (TODO)

> To support user-defined types you need to overload both `__lt__` and `__gt__` operators.
