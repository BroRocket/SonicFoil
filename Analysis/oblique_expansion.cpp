#include <oblique_expansion.hpp>
#include <tools.hpp>

ObliqueExpansion::ObliqueExpansion(double delta_, double M1_, double gamma_)
: delta(delta_), gamma(gamma_), M1(M1_), M2(0), P2_P1(1), T2_T1(1), rho2_rho1(1)
{
    if (M1 <= 1.0) throw std::invalid_argument("Expansion requires M1 > 1");
    M2 = solve_M2(delta);

    T2_T1 = solve_T2_T1(M2);
    P2_P1 = solve_P2_p1(M2);
    rho2_rho1 = solve_rho2_rho1(M2);

};

double ObliqueExpansion::prandtl_meyer(double M){
    return std::sqrt((gamma + 1)/(gamma - 1))*std::atan(std::sqrt(((gamma - 1)/(gamma + 1))*(std::pow(M, 2) - 1))) - std::atan(std::sqrt(std::pow(M, 2) - 1));
};

double ObliqueExpansion::solve_M2(double delta){
    double temp = prandtl_meyer(M1);
     auto f = [&](double M_2) {
        return prandtl_meyer(M_2) - temp - delta;
    };

    double a = M1;
    double b = 20;
    double fa = f(a), fb = f(b);
    if (fa * fb > 0) {
        // try to find a b where sign changes
        b = a * 2.0;
        for (int i=0; i<100 && fa * fb > 0; ++i) {
            fb = f(b);
            if (fa * fb <= 0) break;
            b *= 2.0;
        }
        if (fa * fb > 0) throw std::runtime_error("Cannot bracket root for M2 in prandtl_meyer");
    }

    return solve_bisection(f, a, b);
};

double ObliqueExpansion::solve_P2_p1(double M_2){
    double temp = solve_T2_T1(M_2);
    return std::pow(temp, (gamma/(gamma - 1)));
};

double ObliqueExpansion::solve_T2_T1(double M_2){
    return (1+((gamma - 1)/(2))*std::pow(M1, 2))/(1+((gamma - 1)/(2))*std::pow(M_2, 2));

};

double ObliqueExpansion::solve_rho2_rho1(double M_2){
    double temp = solve_T2_T1(M_2);
    return std::pow(temp, 1/(gamma - 1));
};