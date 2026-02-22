
# SonicFoil
## Description
This program is developped to provide a first-pass development and educational tool for 2D airfoil subsonic and supersonic aerodynamics. It gives the option to employ several different solving technques for lift and drag in both subsonic and superonsic regimes, whihc can help users understand the difference, drawbacks and advantages of different solving methods. The tool includes a modern GUI compared to other similar solvers to be user friendly and easy to manipulate. Utilizing a pyhton front end and cpp backend allows the solver to run quickly while utilizing the madaptability of Python. Hope you enjoy the tool. 

## GUI Windows
Describe the different windows available in the tool

## Solving Methods
Describe the different solving options available in porgram.

## Developper Guide

### Release Build
Describe how to build a new release version

### Backend Build
To rebuild the **.pyd** (essentially a **.dil** file but for python) first ensure all updates to the Cpp code that need to interface with
the Python code are added to **bindings.cpp** and that any new sources are added to the **setup.py** file (also update verison in **setup.py** file). Then in the temrinal change directories to the backend folder and run the folllowing command from the main directory:
```
py -m setup.py build_ext --inplace 
```
This will build the new **sonicfoil_backend.cp310-win_amd64.pyd** and make sure you move it to the root folder to replace the old. For sake of tracking move the old **.pyd** file to the **Previous Builds Folder**.

### Frontend - Backend Interface
Describe how the code interfaces and object trasnfer etc. 