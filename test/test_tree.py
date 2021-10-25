from pytest import raises, main
from pytree import Tree

def test_tree():
	"""  """

	t = Tree()
	t.__init__(range(100))
	t.__init__([5, 1, 4])
	del t

	t = Tree([5, 4])
	assert len(t) == 2
	assert 5 in t
	assert 4 in t
	assert 2 not in t
	assert 6 not in t

	t.add(2)
	t.add(6)
	assert len(t) == 4
	assert 2 in t
	assert 6 in t
	assert 3 not in t

	t.update(range(10))
	for i in range(10):
		assert i in t
	for i in range(10, 100):
		assert i not in t

	t.clear()
	assert len(t) == 0

	t.update(range(10))
	assert len(t) == 10
	for i in range(10):
		assert i in t
		t.remove(i)
		assert i not in t

	for i in range(10, 20):
		with raises(KeyError):
			t.remove(i)

	assert len(t) == 0

	t.update(range(5))
	for i in range(5):
		assert i in t
		t.discard(i)
		assert i not in t

	for i in range(5):
		t.discard(t)

	assert len(t) == 0

	t.update(range(10, 20))
	u = t.copy()
	assert len(u) == 10
	for i in range(10, 20):
		assert i in u

	for i, j in zip(t, range(10, 20)):
		assert i == j

	for i, j in zip(t, u):
		assert i == j

	del t

def test_tree_stress():
	"""  """

	from random import randint

	values = [randint(0, 255) for _ in range(0x1 << 12)]
	t = Tree(values)
	assert [*t] == sorted(values)

	for x in (randint(0, 255) for _ in range(0x1 << 10)):
		if x in t:
			t.remove(x)
		else:
			with raises(KeyError):
				t.remove(x)

	for x in (randint(0, 255) for _ in range(0x1 << 12)):
		t.discard(x)

	t.update(randint(0, 255) for _ in range(0x1 << 16))
	for x in (randint(0, 255) for _ in range(0x1 << 16)):
		t.discard(x)

if __name__ == "__main__":
	exit(main())
