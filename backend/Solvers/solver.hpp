#pragma once
#include "Airfoil/airfoil.hpp"
#include "Solvers/aerodynamic_forces.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

struct FrictionForces {
    double CD;
    double CL;
};

class Result {
    public:
    Result() 
        : wave_solution(std::nullopt), ackeret_solution(std::nullopt), skin_friction_solution_supersonic(std::nullopt) {};

    Result(std::optional<Airfoil> wave, std::optional<Airfoil> ackeret, std::optional<Airfoil> skin_friction) : 
    wave_solution(wave), ackeret_solution(ackeret), skin_friction_solution_supersonic(skin_friction) {};

    std::optional<Airfoil> wave_solution; 
    std::optional<Airfoil> ackeret_solution; 
    std::optional<Airfoil> skin_friction_solution_supersonic;
    std::optional<Airfoil> panel_solution;
    std::optional<Airfoil> skin_friction_solution_subsonic_simple;
    std::optional<Airfoil> skin_friction_solution_subsonic_complex;
    
};

class Solver{
    private:
    Airfoil airfoil_template;
    double gamma;
    

    void ackeret_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void waveshock_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    double HessSmith_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho);

    double iterare_recovery_factor(double r_guess, double s, Segment& seg);
    void skin_friction_supersonic(Airfoil &airfoil, Airfoil &result_airfoil, double alpha);
    FrictionForces compute_surface_skin_friction_supersonic(std::vector<Segment>& segs, double alpha);

    FrictionForces compute_surface_skin_friction_subsonic(std::vector<Segment>& segs, double alpha, double rho_inf, double U_inf, double mu_inf);

    public:
    bool success;
    std::string error_msg;
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