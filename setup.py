from setuptools import setup
import os

import os
setup_path=os.path.dirname(os.path.realpath(__file__))

from pybind11.setup_helpers import build_ext, Pybind11Extension
ext_modules = [
    Pybind11Extension(
        "omprnd",
        ["omprnd.cpp"],
        extra_compile_args=('-fopenmp -DTINA -I'+os.path.join(setup_path,'tina','include')).split(),
        extra_link_args=('-fopenmp -DTINA -L' + os.path.join(setup_path,'tina','lib') + ' -Wl,-Bstatic -ltrng4 -Wl,-Bdynamic ').split(),

    ),
]

setup(name='omprnd',
      version='0.1',
      description='Rndom numbers',
      long_description="Ciao",
      classifiers=['License :: OSI Approved :: GNU General Public License v3 or later (GPLv3+)',
                   'Programming Language :: Python :: 3 :: Only',
                   'Operating System :: OS Independent',
                   'Intended Audience :: Science/Research',
                   'Topic :: Scientific/Engineering :: Physics'],
      url='http://github.com/bonfus/omprnd',
      author='Pietro Bonfa',
      author_email='bonfus@gmail.com',
      license='GPLv3',
      packages=None,
      include_package_data=True,
      install_requires = [
                          'numpy',
                          "pybind11"],
      ext_modules=ext_modules,
      cmdclass={"build_ext": build_ext}
    )
