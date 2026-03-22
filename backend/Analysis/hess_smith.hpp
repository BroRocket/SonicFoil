#pragma once
#include <cmath>
#include <vector>

#include "Tools/matrix.hpp"
#include "Airfoil/airfoil.hpp"

constexpr double PI_ = 3.14159265358979323846;

class HessSmith {
    private:
    
    Airfoil airfoil_template;
    double M;
    double T;
    double V;
    double AoA;
    double gamma;
    // perhaps remove these once you figure out the way to implement best
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> teta;
    std::vector<double> xi;
    std::vector<double> yi;
    Matrix A_ij;
    std::vector<double> b_i;
    std::vector<Segment> airfoil_segments;

    double distance(Cordinate start, Cordinate end);
    void set_state(Segment &seg, double M, double P, double rho, double T);
    
    public:

    std::vector<double> v_ti;
    double kutta_Cl;

    HessSmith(Airfoil &airfoil, double alpha, double M0, double T0, double gamma_ = 1.4);

    Airfoil solve(double P0, double T0, double rho0);

};