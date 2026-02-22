####### TO RUN #### (use in terminal)
# cd backend
# python setup.py build_ext --inplace


from setuptools import setup, Extension
import pybind11
import sys
import os

# ------------------------------
# Detect platform
# ------------------------------
is_windows = sys.platform.startswith("win")
is_macos = sys.platform == "darwin"
is_linux = sys.platform.startswith("linux")

# ------------------------------
# Source files
# ------------------------------
sources = [
    "bindings.cpp",
    "Airfoil/airfoil.cpp",
    "Analysis/oblique_shock.cpp",
    "Analysis/oblique_expansion.cpp",
    "Analysis/hess_smith.cpp",
    "Solvers/solver.cpp",
    "Solvers/aerodynamic_forces.cpp",
    "Tools/matrix.cpp"
]

include_dirs = [
    pybind11.get_include(),
    ".",              # backend root
    "Airfoil",
    "Analysis",
    "Tools",
    "Solvers",
]

# ------------------------------
# OpenMP compiler flags
# ------------------------------
extra_compile_args = []
extra_link_args = []

if is_windows:
    # MSVC OpenMP flag
    extra_compile_args.append("/std:c++17")
    extra_compile_args.append("/openmp")
else:
    # GCC/Clang OpenMP flag
    extra_compile_args.extend(["-std=c++17", "-fopenmp"])
    extra_link_args.append("-fopenmp")

# ------------------------------
# Extension module definition
# ------------------------------
ext_modules = [
    Extension(
        "sonicfoil_backend",
        sources=sources,
        include_dirs=include_dirs,
        language="c++",
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    )
]

# ------------------------------
# Setup
# ------------------------------
setup(
    name="sonicfoil_backend",
    version="0.2",
    ext_modules=ext_modules,
)
