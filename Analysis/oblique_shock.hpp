#pragma once
#include <cmath>

class ObliqueShock{
    private:
    double delta;
    double gamma;
    double M1;

    double solve_epsilon(double delta);
    double solve_M2(double epsilon, double delta);
    double solve_P2_P1();
    double solve_T2_T1();
    double solve_rho2_rho1();

    public:
    double epsilon;
    double M2;
    double P2_P1;
    double T2_T1;
    double rho2_rho1;
    
    // All angles in radians
    ObliqueShock(double delta_, double M1_, double gamma_ = 1.4);
   
};