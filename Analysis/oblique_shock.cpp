#include <oblique_shock.hpp>
#include <tools.hpp>

#include <iostream>

ObliqueShock::ObliqueShock(double delta_, double M1_, double gamma_) 
: delta(delta_), gamma(gamma_), M1(M1_), epsilon(0.0), M2(0.0), P2_P1(1.0), T2_T1(1.0), rho2_rho1(1.0) 
    {

    if (M1 <= 1.0) throw std::invalid_argument("ObliqueShock: M1 must be > 1");

    epsilon = solve_epsilon(delta);
    M2 = solve_M2(epsilon, delta);
    P2_P1 = solve_P2_P1();
    T2_T1 = solve_T2_T1();
    rho2_rho1 = solve_rho2_rho1();

};

double ObliqueShock::solve_epsilon(double delta){
    double target = std::tan(delta);

    auto f = [&](double epsilon) {
        return (2.0/std::tan(epsilon)) *
               ((M1*M1*std::pow(std::sin(epsilon), 2) - 1.0) /
                (M1*M1*(gamma + std::cos(2*epsilon)) + 2.0))
               - target;
    };

    // Start near Mach angle
    double eps_left = std::asin(1.0/M1) + 1e-6;
    double eps_right = 3.14159/2 - 1e-3; //set to pi

    // Try to locate a sign change by scanning
    const int steps = 300;
    double prev = f(eps_left);
    for(int i = 1; i <= steps; i++){
        double e = eps_left + (eps_right - eps_left)*i/steps;
        double val = f(e);
        if(prev * val < 0){ 
            return solve_bisection(f, e-(2*((eps_right - eps_left)/steps)), e); // e-2*((eps_right - eps_left)/steps)
        }
        prev = val;
    }

    throw std::runtime_error("No valid oblique shock solution for this delta and M1 (delta may exceed delta_max).");
};

double ObliqueShock::solve_M2(double epsilon, double delta){
    double M2_sqrd = ((std::pow(M1, 2)*std::pow(sin(epsilon),2)) + (2/(gamma - 1)))/(std::pow(sin(epsilon - delta), 2)*((((2*gamma)/(gamma - 1))*std::pow(M1 , 2)*std::pow(sin(epsilon), 2)) - 1));
    return sqrt(M2_sqrd);
};

double ObliqueShock::solve_P2_P1(){
    return 1 + ((2*gamma)/(gamma + 1))*(std::pow(M1, 2)*std::pow(sin(epsilon), 2) - 1);
};

double ObliqueShock::solve_T2_T1(){
    return ((((2*gamma)/(gamma + 1))*std::pow(M1, 2)*std::pow(sin(epsilon), 2)) - ((gamma - 1)/(gamma + 1)))*(((gamma - 1)/(gamma + 1)) + (2/((gamma + 1)*std::pow(M1, 2)*std::pow(sin(epsilon), 2))));
};

double ObliqueShock::solve_rho2_rho1(){
    return tan(epsilon)/tan(epsilon-delta);
};