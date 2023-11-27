import setuptools.command.egg_info
from setuptools import setup, Extension, find_packages
import distutils.command.build
from Cython.Build import cythonize
import os

def get_build_dir(default):
    return os.environ.get('STFPY_BUILD_DIR', default)

# Override egg command
class EggCommand(setuptools.command.egg_info.egg_info):
    def initialize_options(self):
        setuptools.command.egg_info.egg_info.initialize_options(self)
        self.egg_base = get_build_dir(self.egg_base)

# Override build command
class BuildCommand(distutils.command.build.build):
    def initialize_options(self):
        distutils.command.build.build.initialize_options(self)
        self.build_base = get_build_dir(self.build_base)

if __name__ == '__main__':
    setup(
        name = "stfpy",
        packages = find_packages(),
        cmdclass = {'build': BuildCommand, 'egg_info': EggCommand},
        ext_modules = cythonize(Extension('*',
                                          sources=["stfpy/*.pyx"],
                                          language='c++',
                                          extra_link_args=os.environ.get('LDFLAGS', '').split(' ')), # Ensure our link flags come last
                                nthreads = 4,
                                language_level = "3")
    )
