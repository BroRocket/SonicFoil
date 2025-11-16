#include <aerodynamic_forces.hpp>

AerodynamicForces::AerodynamicForces(Airfoil &airfoil_, double AoA) : airfoil(airfoil), alpha(AoA) {

    // get lift, drag, CL, Cd, and Cm about quarter chord and xcp and Cm about cp 

    // calculate F top then F bottom then F forward then F back
    double Normal_force_top;
    double Normal_force_bottom;
    double Axial_force_front;
    double Axial_force_back;

    for (size_t i = 0; i < airfoil.top_segments.size(); ++i){
        Normal_force_top += airfoil.top_segments[i].state.p * std::cos(airfoil.top_segments[i].angle) * airfoil.top_segments[i].x_distance;
        if (airfoil.top_segments[i].angle > 0) {
            Axial_force_front += airfoil.top_segments[i].state.p * std::sin(airfoil.top_segments[i].angle) * airfoil.top_segments[i].y_distance;
        } else if (airfoil.top_segments[i].angle < 0) {
            Axial_force_back += airfoil.top_segments[i].state.p * std::sin(airfoil.top_segments[i].angle) * airfoil.top_segments[i].y_distance;
        };
    };

    for (size_t i = 0; i < airfoil.bottom_segments.size(); ++i){
        Normal_force_bottom += airfoil.bottom_segments[i].state.p * std::cos(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].x_distance;
         if (airfoil.bottom_segments[i].angle > 0) {
            Axial_force_back += airfoil.bottom_segments[i].state.p * std::sin(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].y_distance;
        } else if (airfoil.bottom_segments[i].angle < 0) {
            Axial_force_front += airfoil.bottom_segments[i].state.p * std::sin(airfoil.bottom_segments[i].angle) * airfoil.bottom_segments[i].y_distance;
        };
    };
    
    double Normal_force = Normal_force_top - Normal_force_bottom;
    double Axial_force = Axial_force_front - Axial_force_back;

    double Lift = std::cos(alpha) * Normal_force - std::sin(alpha) * Axial_force;
    double Drag = std::sin(alpha) * Normal_force + std::cos(alpha) * Axial_force;
    
    

};

