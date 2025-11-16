#pragma once
#include <airfoil.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <iostream>



class AerodynamicForces{
    private:
    Airfoil airfoil;
    double alpha; // angle of attakc
    

    public:
    // All angles in radians
    // expects to be in terms of chord
    AerodynamicForces(Airfoil &airfoil_, double AoA);

    void print_forces(){
        
    };
   
};