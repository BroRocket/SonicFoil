from setuptools import setup, Extension
import pybind11
import sys
import os

# Path to your C++ source files
sources = [
    "bindings.cpp",
    "Airfoil/airfoil.cpp",
    "Analysis/oblique_shock.cpp",
    "Analysis/oblique_expansion.cpp",
    "Solvers/solver.cpp",
    "Solvers/aerodynamic_forces.cpp",
]

include_dirs = [
    pybind11.get_include(),
    "Airfoil",
    "Analysis",
    "Tools",
    "Solvers",
]

ext_modules = [
    Extension(
        "sonicfoil_backend",      # module name
        sources=sources,
        include_dirs=include_dirs,
        language="c++",
        extra_compile_args=["/std:c++17"],
    )
]

setup(
    name="sonicfoil_backend",
    version="0.1",
    ext_modules=ext_modules,
)