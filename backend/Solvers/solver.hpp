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
        : wave_solution(std::nullopt), ackeret_solution(std::nullopt), skin_friction_solution_supersonic(std::nullopt), panel_forces_solution(std::nullopt), panel_kutta_solution(std::nullopt), skin_friction_solution_subsonic(std::nullopt) {};

    Result(std::optional<Airfoil> wave, std::optional<Airfoil> ackeret, std::optional<Airfoil> skin_friction_supersonic, std::optional<Airfoil> panel_forces, std::optional<Airfoil> panel_kutta, std::optional<Airfoil> skin_friction_subsonic) : 
    wave_solution(wave), ackeret_solution(ackeret), skin_friction_solution_supersonic(skin_friction_supersonic), panel_forces_solution(panel_forces), panel_kutta_solution(panel_kutta), skin_friction_solution_subsonic(skin_friction_subsonic) {};

    std::optional<Airfoil> wave_solution; 
    std::optional<Airfoil> ackeret_solution; 
    std::optional<Airfoil> skin_friction_solution_supersonic;
    std::optional<Airfoil> panel_forces_solution;
    std::optional<Airfoil> panel_kutta_solution;
    std::optional<Airfoil> skin_friction_solution_subsonic; 
};

class Solver{
    private:
    Airfoil airfoil_template;
    double gamma;
    double R;
    

    void ackeret_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void waveshock_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    double HessSmith_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0);
    void set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho);
    
    void skin_friction_supersonic(Airfoil &airfoil, Airfoil &result_airfoil, double alpha);
    FrictionForces compute_surface_skin_friction_supersonic(const std::vector<Segment>& segs, double alpha);
    double iterate_recovery_factor(double r_guess, double s, const Segment& seg);

    void skin_friction_subsonic(Airfoil &airfoil, Airfoil &result_airfoil, double alpha);
    FrictionForces compute_surface_skin_friction_subsonic(std::vector<Segment>& segs, double alpha);

    public:
    bool success;
    std::string error_msg;
    std::vector<Result> Results;
    // All angles in radians
    // all values must be static values
    Solver(Airfoil &airfoil_template, double gamma_ = 1.4, double R_ = 287);

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