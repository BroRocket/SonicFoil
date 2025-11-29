#pragma once
#include <airfoil.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <iostream>



class AerodynamicForces{
    private:
    Airfoil &airfoil;
    double alpha; // angle of attakc
    double M0;
    double P0;
    double gamma;

    public:
    // All angles in radians
    // expects to be in terms of chord
    AerodynamicForces(Airfoil &airfoil_, double AoA, double P, double M, double gamma_ = 1.4);

    void print_forces(){
        
    };
   
};