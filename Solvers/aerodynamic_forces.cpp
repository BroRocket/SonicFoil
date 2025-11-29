#include <aerodynamic_forces.hpp>

AerodynamicForces::AerodynamicForces(Airfoil &airfoil_, double AoA, double P, double M, double gamma_) 
: airfoil(airfoil_), alpha(AoA), M0(M), P0(P), gamma(gamma_) {

    // get lift, drag, CL, Cd, and Cm about quarter chord and xcp and Cm about cp 
    // calculate F top then F bottom then F forward then F back
    double xforce = 0;
    double yforce = 0;
    double Normal_force = 0;
    double Axial_force = 0;
    
    double horizontal_distance_sum = 0;
    double x_moment = 0;
    double vertical_distance_sum = 0;
    double y_moment = 0;

    for (size_t i = 0; i < airfoil.top_segments.size(); ++i){
        xforce = -1 * airfoil.top_segments[i].state.p * std::cos(airfoil.top_segments[i].angle) * airfoil.top_segments[i].x_distance;
        Normal_force += xforce;

        if (airfoil.top_segments[i].angle >= 0) {
            yforce = airfoil.top_segments[i].state.p * std::sin(airfoil.top_segments[i].angle) * airfoil.top_segments[i].y_distance;
        } else if (airfoil.top_segments[i].angle < 0) {
            yforce = -1 * airfoil.top_segments[i].state.p * std::sin(airfoil.top_segments[i].angle) * airfoil.top_segments[i].y_distance;
        };
        Axial_force += yforce;

        y_moment += yforce * (vertical_distance_sum + (airfoil.top_segments[i].y_distance / 2));
        vertical_distance_sum += airfoil.top_segments[i].y_distance;
        x_moment += xforce * (horizontal_distance_sum + (airfoil.top_segments[i].x_distance / 2));
        horizontal_distance_sum += airfoil.top_segments[i].x_distance;
    };

    horizontal_distance_sum = 0;
    vertical_distance_sum = 0;

    for (size_t i = 0; i < airfoil.bottom_segments.size(); ++i){
        xforce = airfoil.bottom_segments[i].state.p * std::cos(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].x_distance;
        Normal_force += xforce;

        if (airfoil.bottom_segments[i].angle >= 0) {
            yforce = -1 * airfoil.bottom_segments[i].state.p * std::sin(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].y_distance;
        } else if (airfoil.bottom_segments[i].angle < 0) {
            yforce = airfoil.bottom_segments[i].state.p * std::sin(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].y_distance;
        };
        Axial_force += yforce;

        y_moment += yforce * (vertical_distance_sum + (airfoil.bottom_segments[i].y_distance / 2));
        vertical_distance_sum += airfoil.bottom_segments[i].y_distance;
        x_moment += xforce * (horizontal_distance_sum + (airfoil.bottom_segments[i].x_distance / 2));
        horizontal_distance_sum += airfoil.bottom_segments[i].x_distance;
    };

    double Lift = std::cos(alpha) * Normal_force - std::sin(alpha) * Axial_force;
    double Drag = std::sin(alpha) * Normal_force + std::cos(alpha) * Axial_force;

    if (std::abs(Normal_force) < 1e-12) { 
        airfoil.Forces.x_cp = 0;
    } else {
        airfoil.Forces.x_cp = x_moment / Normal_force;
    };
     if (std::abs(Axial_force) < 1e-12) { 
        airfoil.Forces.y_cp = 0;
    } else {
        airfoil.Forces.y_cp = y_moment / Axial_force;
    };
    
    double M_LE = -1 * airfoil.Forces.x_cp * Normal_force;

    // dimensionless
    double q = (gamma/2) * P0 * M0 * M0;

    airfoil.Forces.CL = Lift / q;
    airfoil.Forces.Cd = Drag / q;
    airfoil.Forces.C_MLE = M_LE / q;
    airfoil.Forces.CL_Cd = airfoil.Forces.CL / airfoil.Forces.Cd;

};

