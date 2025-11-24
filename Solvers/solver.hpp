#pragma once
#include <airfoil.hpp>
#include <aerodynamic_forces.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <iostream>


struct FrictionForces {
    double CD;
    double CL;
};

class Solver{
    private:
    Airfoil airfoil_template;
    std::vector<Airfoil> airfoils;
    double gamma;
    double alpha; // angle of attack
    double M0;
   
    double P0;
    double T0;
    double rho0;

    void ackeret_method(Airfoil &airfoil);
    void waveshock_method(Airfoil &airfoil);
    void set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho);

    double iterare_recovery_factor(double r_guess, double s, Segment seg);
    void skin_friction(Airfoil &airfoil, Airfoil &result_airfoil);
    FrictionForces compute_surface_skin_friction(std::vector<Segment>& segs);

    public:
    // All angles in radians
    // all values must be static values
    Solver(Airfoil &airfoil, std::string method, bool drag, double AoA, double M, double p, double T, double rho, double gamma_ = 1.4);

    void print_solutions(){
        for (size_t i = 0; i < airfoils.size(); ++i){
            std::cout << "Airfoil " << i + 1 << ":\n";
            airfoils[i].print_airfoil();
        }
    };
   
};