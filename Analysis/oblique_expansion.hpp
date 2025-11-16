#pragma once
#include <cmath>

class ObliqueExpansion {
    private:
    double delta;
    double gamma;
    double M1;
    
    double prandtl_meyer(double M);
    double solve_M2(double delta);
    double solve_P2_p1(double M_2);
    double solve_T2_T1(double M_2);
    double solve_rho2_rho1(double M_2);

    public:
    double P2_P1;
    double T2_T1;
    double rho2_rho1;
    double M2;

    ObliqueExpansion(double delta_, double M1_, double gamma_ = 1.4);

};