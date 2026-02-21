#pragma once
#include <cmath>
#include <vector>

#include "Tools/matrix.hpp"
#include "Airfoil/airfoil.hpp"

class HessSmith {
    private:
    
    Airfoil airfoil_template;
    double M;
    double T;
    double V;
    double AoA;
    Matrix A_ij;
    std::vector<double> b_i;
    std::vector<Segment> airfoil_segments;

    double distance(Cordinate start, Cordinate end);
    void HessSmith::set_state(Segment &seg, double M, double P, double rho, double T);
    
    public:

    std::vector<double> v_ti;
    double kutta_Cl;

    HessSmith(Airfoil &airfoil, double alpha, double M0, double T0);

    Airfoil solve(double P0, double T0, double rho0, double gamma = 1.4, double R = 287);

};