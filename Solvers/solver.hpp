#pragma once
#include <airfoil.hpp>
#include <aerodynamic_forces.hpp>

#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

//FIX SKIN FRICITON DRAG AS I'M NOT SURE IT DOES WHAT i WANT NO LIFT DRAG AT ALL ANGLES AND SKIN FRICITON STAYS THE SAME

struct FrictionForces {
    double CD;
    double CL;
};

class Result {
    public:
    Result() 
        : wave_solution(std::nullopt), ackeret_solution(std::nullopt), skin_friction_solution(std::nullopt) {};

    Result(std::optional<Airfoil> wave, std::optional<Airfoil> ackeret, std::optional<Airfoil> skin_friction) : 
    wave_solution(wave), ackeret_solution(ackeret), skin_friction_solution(skin_friction) {};

    std::optional<Airfoil> wave_solution; 
    std::optional<Airfoil> ackeret_solution; 
    std::optional<Airfoil> skin_friction_solution;
    
};

class Solver{
    private:
    Airfoil airfoil_template;
    double gamma;
    

    void ackeret_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void waveshock_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho);

    double iterare_recovery_factor(double r_guess, double s, Segment seg);
    void skin_friction(Airfoil &airfoil, Airfoil &result_airfoil, double alpha);
    FrictionForces compute_surface_skin_friction(std::vector<Segment>& segs, double alpha);

    public:
    std::vector<Result> Results;
    // All angles in radians
    // all values must be static values
    Solver(Airfoil &airfoil_template, double gamma_ = 1.4);

    Result solve_single(std::string method, double AoA, double M0, double P0, double T0, double rho0);

    void solve_range(std::string method, const std::vector<double>& angles, double M0, double P0, double T0, double rho0);

    //Solver(Airfoil &airfoil, std::string method, bool skin_drag, double AoA, double M, double p, double T, double rho, double gamma_ = 1.4);

    void print_solutions(){
        for (size_t i = 0; i < Results.size(); ++i){
            std::cout << "Airfoil " << i + 1 << ":\n";
            // if (Results[i].wave_solution.has_value()){
            //     Results[i].wave_solution.print_airfoil()
            // };
            
            // Results[i].ackeret_solution.print_airfoil();
            // Results[i].skin_friction_solution.print_airfoil();
        };
    };
   
};