from random import randint
from pytest import raises, main
from pyctree import SortedSet, Tree

def test_sorted_set():
	"""  """

	s = SortedSet()
	assert len(s) == 0
	assert 1 not in s
	assert 10 not in s
	assert isinstance(s, Tree)
	del s

	s = SortedSet(range(10))
	assert len(s) == 10
	for x in (randint(0, 99) for _ in range(0x100)):
		assert (x in range(10) and x in s) or (x not in range(10) and x not in s)
	for x in (randint(0, 99) for _ in range(0x100)):
		assert (x in range(10) and s.get(x) == x) or (x not in range(10) and s.get(x) is None)

	s.add(10)
	assert len(s) == 11
	for x in range(11):
		assert x in s

	s = SortedSet()
	s.update(range(256))
	assert len(s) == 256
	for x in (randint(0, 1024) for _ in range(0x1000)):
		assert (x in range(256) and x in s) or (x not in range(256) and x not in s)

	s.update(randint(0, 255) for _ in range(0x1000))
	assert len(s) == 256

	for x in range(0, 256, 3):
		assert x in s
		s.remove(x)
		assert x not in s
	assert len(s) == 170

	s.clear()
	assert len(s) == 0
	for x in range(256):
		assert x not in s

	s.update(range(64))
	assert [*s] == list(range(64))

	for x, y in zip(s, range(64)):
		assert x == y

	for x in (randint(0, 1024) for _ in range(0x1000)):
		if x in s:
			s.remove(x)
		else:
			with raises(KeyError):
				s.remove(x)
		assert x not in s

	s.update(range(64))
	for x in (randint(0, 1024) for _ in range(0x1000)):
		s.discard(x)

	s.update(range(64))
	assert len(s) == 64
	for x in range(256):
		s.discard(x)
		assert x not in s
	assert len(s) == 0

	s.update(range(256))
	r = s.copy()
	assert len(r) == len(s)
	for x in s:
		assert x in r
	for x, y in zip(s, r):
		assert x == y
	s.clear()
	assert len(r) == 256
	for x in range(256):
		assert x in r

if __name__ == "__main__":
	exit(main())
