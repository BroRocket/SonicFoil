#pragma once
#include "Airfoil/airfoil.hpp"
#include "Solvers/aerodynamic_forces.hpp"
#include "Analysis/XfoilWrapper.hpp"

#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <iostream>

struct FrictionForces {
    double CD;
    double CL;
    double C_Mle;
};

class Result {
    public:
    Result() 
        : wave_solution(std::nullopt), ackeret_solution(std::nullopt), skin_friction_solution_supersonic(std::nullopt), xfoil_invicid_solution(std::nullopt), xfoil_viscid_solution(std::nullopt) {};

    Result(std::optional<Airfoil> wave, std::optional<Airfoil> ackeret, std::optional<Airfoil> skin_friction_supersonic, std::optional<Airfoil> xfoil_inviscid, std::optional<Airfoil> xfoil_viscid) : 
    wave_solution(wave), ackeret_solution(ackeret), skin_friction_solution_supersonic(skin_friction_supersonic), xfoil_invicid_solution(xfoil_inviscid), xfoil_viscid_solution(xfoil_viscid) {};

    std::optional<Airfoil> wave_solution; 
    std::optional<Airfoil> ackeret_solution; 
    std::optional<Airfoil> skin_friction_solution_supersonic;
    std::optional<Airfoil> xfoil_invicid_solution;
    std::optional<Airfoil> xfoil_viscid_solution; 
};

class Solver{
    private:
    Airfoil airfoil_template;
    double gamma;
    double R;
    XfoilWrapper XFoil = XfoilWrapper();
    
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
   
};