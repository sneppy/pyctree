#!/usr/bin/env python
"""
PyCTree is a collection of tree-based containers for Python, written in C.
"""

from distutils.core import Extension, setup

# The PyCTree module definition
pyctreemodule = Extension(
	name="pyctree",
	sources=["src/pyctreemodule.c",
			 "src/pyctree_tree.c",
			 "src/tree.c"],
	include_dirs=["include/"]
)

# Setup PyCTree package
setup(
	name="pyctree",
	version="0.1.6",
	description=__doc__,
	author="Andrea Mecchia",
	author_email="andrea.mecchia@arm.com",
	url="https://github.com/sneppy/pyctree",
	license="MIT",
	classifiers=[
		"License :: OSI Approved :: MIT License",
        "Intended Audience :: Developers",
		"Programming Language :: Python :: 3",
		"Programming Language :: Python :: 3.6",
		"Programming Language :: Python :: 3.7",
		"Programming Language :: Python :: 3.8",
	],
	ext_modules=[pyctreemodule]
)
