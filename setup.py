""" TODO """

from distutils.core import setup, Extension

# The PyTree module definition
pytreemodule = Extension(name='pytree',
                         sources=['src/pytreemodule.c', 'src/tree.c',
						          'src/pytree_tree.c'],
						 include_dirs=['include/'])

# Setup PyTree package
setup(name='pytree',
      version='0.1.1',
	  description=__doc__,
	  author='Andrea Mecchia',
	  author_email='andrea.mecchia@arm.com',
	  ext_modules=[pytreemodule])