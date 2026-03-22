#include "Airfoil/airfoil.hpp"
#include "Analysis/oblique_shock.hpp"
#include "Analysis/oblique_expansion.hpp"
#include "Solvers/solver.hpp"

#include <iostream>
#include <math.h>

int main(){

    Airfoil test_airfoil("C:/Users/Brody Howard/Documents/GitHub/SonicFoil/Airfoil Files/flat_diamond.dat");
    Solver test_solver(test_airfoil);

    Result res = test_solver.solve_single("i", 10* M_PI / 180, 0.2, 101000, 285, 1.225);
    Airfoil foil = res.xfoil_invicid_solution.value();
    std::cout << "Inviscid CL = " << foil.Forces.CL << " Inviscid Cd = " << foil.Forces.Cd << " Inviscid Cm = " << foil.Forces.C_MLE << " Inviscid CL/Cd = " << foil.Forces.CL_Cd;
    

    // Airfoil test_airfoil("C:/Users/Brody Howard/Documents/GitHub/SonicFoil/Airfoil Files/test.dat");
    // //stest_airfoil.print_airfoil();
    // Solver test_solver(test_airfoil);
    // //test_solver.solve_single("w", 20*M_PI/180, 2.3, 101000, 275, 1.225);
    // std::vector<double> angles;
    // double max_angle = 15 * M_PI / 180;
    // double step_size = 0.005 * M_PI / 180;
    // double temp = 0;
    // while (temp < max_angle) {
    //     angles.push_back(temp);
    //     temp = temp + step_size;
    // };
    // angles.push_back(max_angle);

    // test_solver.solve_range("w", angles, 2.3, 101000, 275, 1.225);
    //test_solver.print_solutions();
    

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
