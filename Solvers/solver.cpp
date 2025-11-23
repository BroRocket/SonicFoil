#include <solver.hpp>
#include <oblique_expansion.hpp>
#include <oblique_shock.hpp>
#include <aerodynamic_forces.hpp>

Solver::Solver(Airfoil &airfoil, std::string method, double AoA, double M, double p, double T, double rho, double gamma_) 
: alpha(AoA), M0(M), P0(p), T0(T), rho0(rho), gamma(gamma_), airfoil_template(airfoil)
 {
    if (M0 < 1) throw std::invalid_argument("The freestream Mach Number must be larger than 1.");
    if (method == "w") {
        Airfoil waveAirfoil = airfoil_template;
        //solve first as then the modified arifoil copy is stored in vector
        waveshock_method(waveAirfoil);
        AerodynamicForces(waveAirfoil, alpha, P0, M0);
        airfoils.push_back(waveAirfoil);
        // Then do friction calculations

    } else if ( method == "a") {
        Airfoil ackeretfoil = airfoil_template;
        ackeret_method(ackeretfoil);
        AerodynamicForces(ackeretfoil, alpha, P0, M0);
        airfoils.push_back(ackeretfoil);

    } else if (method == "b") {
        Airfoil waveAirfoil = airfoil_template;
        //solve first as then the modified arifoil copy is stored in vector
        waveshock_method(waveAirfoil);
        AerodynamicForces(waveAirfoil, alpha, P0, M0);
        airfoils.push_back(waveAirfoil);
        Airfoil ackeretfoil = airfoil_template;
        ackeret_method(ackeretfoil);
        AerodynamicForces(ackeretfoil, alpha, P0, M0);
        airfoils.push_back(ackeretfoil);

    } else {
        throw std::invalid_argument("Invalid Method");
    };

};

void Solver::waveshock_method(Airfoil &airfoil) {
    //loop top airfoil
    double current_flow_angle = alpha;
    double M_prev = M0;
    double P_prev = P0;
    double T_prev = T0;
    double rho_prev = rho0;

    for (size_t i = 0; i < airfoil.top_segments.size(); ++i) {
        if (current_flow_angle >= 0) {
            if (airfoil.top_segments[i].angle < current_flow_angle){
                // flow has positive angle of attack and incoming dsegment has an angle less than that of the flow
                // expansion wave would form solve delta
                double delta = current_flow_angle - airfoil.top_segments[i].angle;
                ObliqueExpansion expansion_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.top_segments[i], expansion_wave.M2, expansion_wave.P2_P1*P_prev, expansion_wave.T2_T1*T_prev, expansion_wave.rho2_rho1*rho_prev);
                
            } else if (airfoil.top_segments[i].angle > current_flow_angle) {
                // compression wave would form solve for delta
                double delta = airfoil.top_segments[i].angle - current_flow_angle;
                ObliqueShock compression_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.top_segments[i], compression_wave.M2, compression_wave.P2_P1*P_prev, compression_wave.T2_T1*T_prev, compression_wave.rho2_rho1*rho_prev);

            } else {
                // nothing happens as flow is not changing direction flow conditions are same as the previosu segment
                    set_segment_state(airfoil.top_segments[i], M_prev, P_prev, T_prev, rho_prev);
            };
        } else if (current_flow_angle < 0) {
            if (airfoil.top_segments[i].angle > current_flow_angle){
                // flow has negative angle of attack and the flow is being delfected towards segment so oblique shock will form
                double delta = airfoil.top_segments[i].angle - current_flow_angle;
                ObliqueShock compression_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.top_segments[i], compression_wave.M2, compression_wave.P2_P1*P_prev, compression_wave.T2_T1*T_prev, compression_wave.rho2_rho1*rho_prev);

            } else if (airfoil.top_segments[i].angle < current_flow_angle) {
                // compression wave would form solve for delta
                double delta = current_flow_angle - airfoil.top_segments[i].angle;
                ObliqueExpansion expansion_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.top_segments[i], expansion_wave.M2, expansion_wave.P2_P1*P_prev, expansion_wave.T2_T1*T_prev, expansion_wave.rho2_rho1*rho_prev);
            
            } else {
                // nothing happens as flow is not changing direction flow conditions are same as the previosu segment
                set_segment_state(airfoil.top_segments[i], M_prev, P_prev, T_prev, rho_prev);
            };
        }
        //update for next iteration
        M_prev = airfoil.top_segments[i].state.M;
        P_prev = airfoil.top_segments[i].state.p;
        T_prev = airfoil.top_segments[i].state.T;
        rho_prev = airfoil.top_segments[i].state.rho;
        current_flow_angle = airfoil.top_segments[i].angle;
    }

    current_flow_angle = alpha;
    M_prev = M0;
    P_prev = P0;
    T_prev = T0;
    rho_prev = rho0;

    // loop bottom of airofil
    for (size_t i = 0; i < airfoil.bottom_segments.size(); ++i) {
        if (current_flow_angle >= 0) {
            if (airfoil.bottom_segments[i].angle < current_flow_angle){
                // flow has positive angle of attack and incoming dsegment has an angle less than that of the flow
                // expansion wave would form solve delta
                double delta = current_flow_angle - airfoil.bottom_segments[i].angle;
                ObliqueShock compression_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.bottom_segments[i], compression_wave.M2, compression_wave.P2_P1*P_prev, compression_wave.T2_T1*T_prev, compression_wave.rho2_rho1*rho_prev);
                
            } else if (airfoil.bottom_segments[i].angle > current_flow_angle) {
                // compression wave would form solve for delta
                double delta = airfoil.bottom_segments[i].angle - current_flow_angle;
                ObliqueExpansion expansion_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.bottom_segments[i], expansion_wave.M2, expansion_wave.P2_P1*P_prev, expansion_wave.T2_T1*T_prev, expansion_wave.rho2_rho1*rho_prev);

            } else {
                // nothing happens as flow is not changing direction flow conditions are same as the previosu segment
                    set_segment_state(airfoil.bottom_segments[i], M_prev, P_prev, T_prev, rho_prev);
            };
        } else if (current_flow_angle < 0) {
            if (airfoil.bottom_segments[i].angle > current_flow_angle){
                // flow has negative angle of attack and the flow is being delfected towards segment so oblique shock will form
                double delta = airfoil.bottom_segments[i].angle - current_flow_angle;
                ObliqueExpansion expansion_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.bottom_segments[i], expansion_wave.M2, expansion_wave.P2_P1*P_prev, expansion_wave.T2_T1*T_prev, expansion_wave.rho2_rho1*rho_prev);

            } else if (airfoil.bottom_segments[i].angle < current_flow_angle) {
                // compression wave would form solve for delta
                double delta = current_flow_angle - airfoil.bottom_segments[i].angle;
                ObliqueShock compression_wave(delta, M_prev, gamma);
                set_segment_state(airfoil.bottom_segments[i], compression_wave.M2, compression_wave.P2_P1*P_prev, compression_wave.T2_T1*T_prev, compression_wave.rho2_rho1*rho_prev);
            
            } else {
                // nothing happens as flow is not changing direction flow conditions are same as the previosu segment
                set_segment_state(airfoil.bottom_segments[i], M_prev, P_prev, T_prev, rho_prev);
            };
        };
        //update for next iteration
        M_prev = airfoil.bottom_segments[i].state.M;
        P_prev = airfoil.bottom_segments[i].state.p;
        T_prev = airfoil.bottom_segments[i].state.T;
        rho_prev = airfoil.bottom_segments[i].state.rho;
        current_flow_angle = airfoil.bottom_segments[i].angle;
    }

    //loop bottom airfoil
};

// Needs to be fixed either angle delta is wrong maybe remove negatives and/or P calc is wrong leave as Cp
void Solver::ackeret_method(Airfoil &airfoil) {

    for (Segment &seg : airfoil.top_segments){
        double delta = seg.angle - alpha;   // Correct sign convention
        double Cp = ( 2.0 * delta ) / std::sqrt(M0*M0 - 1.0);
        double P  = P0 * (1.0 + ((gamma*M0*M0*Cp)/2));
     // Use static freestream pressure

        set_segment_state(seg, M0, P, T0, rho0);
    }

    for (Segment &seg : airfoil.bottom_segments){
        double delta = seg.angle - alpha;   // Same formula, sign handles itself
        double Cp = ( 2.0 * delta ) / std::sqrt(M0*M0 - 1.0);
        double P  = P0 * (1.0 + ((gamma*M0*M0*Cp)/2));

        set_segment_state(seg, M0, P, T0, rho0);
    };
}


void Solver::set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho) {
    airfoil_segment.state.M = M;
    airfoil_segment.state.p = P;
    airfoil_segment.state.T = T;
    airfoil_segment.state.rho = rho;
};
