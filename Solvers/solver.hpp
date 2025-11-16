#pragma once
#include <airfoil.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <iostream>


class Solver{
    private:
    Airfoil airfoil_template;
    std::vector<Airfoil> airfoils;
    double gamma;
    double alpha; // angle of attack
    double M0;
    // all values must be static values
    double P0;
    double T0;
    double rho0;

    void akeret_method(Airfoil &airfoil);
    void waveshock_method(Airfoil &airfoil);
    void set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho);

    public:
    // All angles in radians
    Solver(Airfoil &airfoil, std::string method, double AoA, double M, double p, double T, double rho, double gamma_ = 1.4);

    void print_solutions(){
        for (size_t i = 0; i < airfoils.size(); ++i){
            std::cout << "Airfoil " << i + 1 << ":\n";
            airfoils[i].print_airfoil();
        }
    };
   
};