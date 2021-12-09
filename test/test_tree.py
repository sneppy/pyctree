from random import randint
from pytest import raises, main
from pyctree import Tree


def test_Tree():
    """
    Generic test for major functionalities of the
    Tree class.
    """

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


def test_Tree_bounds():
    """
    Test the methods to get the bounds in a tree.
    """

    t = Tree([1, 1, 2, 3, 7, 6, 10, 11, 15])

    expect_lb = [1, 1, 2, 3, 6, 6, 6, 7, 10, 10, 10, 11, 15, 15, 15, 15, None, None, None, None]
    expect_rb = [None, 1, 2, 3, 3, 3, 6, 7, 7, 7, 10, 11, 11, 11, 11, 15, 15, 15, 15, 15]

    for n in range(20):
        assert t.left_bound(n) == expect_lb[n]
        assert t.right_bound(n) == expect_rb[n]


def test_Tree_stress():
    """  """

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
