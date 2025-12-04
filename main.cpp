#include "Airfoil/airfoil.hpp"
#include "Analysis/oblique_shock.hpp"
#include "Analysis/oblique_expansion.hpp"
#include "Solvers/solver.hpp"

#include <iostream>

int main(){

    Airfoil test_airfoil("C:/Users/Brody Howard/Documents/GitHub/SonicFoil/Airfoil Files/test.dat");
    test_airfoil.print_airfoil();
    Solver test_solver(test_airfoil, "w", true, 0.15, 2.5, 200000, 280, 1.255);
    test_solver.print_solutions();
    

    // ObliqueShock shock(test.top_segments[0].angle, 1.7);

    // double T1 = 280;
    // double P1 = 70000;
    // double rho1 = 1.225;

    // double T2 = T1 * shock.T2_T1;
    // double P2 = P1 * shock.P2_P1;
    // double rho2 = rho1 * shock.rho2_rho1;
    // double M2 = shock.M2;

    // std::cout << "SHOCK: Epsilon = " << shock.epsilon << ", M2 = " << M2 << ", P2 = " << P2 << ", T2 = " << T2 << ", rho2 = " << rho2 << "\n";

    // ObliqueExpansion expansion(test.top_segments[0].angle + -1 * test.top_segments[1].angle, 1.7);

    // T2 = T1 * expansion.T2_T1;
    // P2 = P1 * expansion.P2_P1;
    // rho2 = rho1 * expansion.rho2_rho1;
    // M2 = expansion.M2;

    // std::cout << "Expansion:" << " M2 = " << M2 << ", P2 = " << P2 << ", T2 = " << T2 << ", rho2 = " << rho2 << "\n";

    return 0;
};
