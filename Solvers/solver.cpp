#include <solver.hpp>
#include <oblique_expansion.hpp>
#include <oblique_shock.hpp>
#include <aerodynamic_forces.hpp>

#include <cmath>
#include <vector>

Solver::Solver(Airfoil &airfoil, std::string method, bool drag, double AoA, double M, double p, double T, double rho, double gamma_) 
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
        if (drag == true) {
            Airfoil dragfoil = airfoil_template;
            skin_friction(waveAirfoil, dragfoil);
            airfoils.push_back(dragfoil);
        };

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

        if (drag == true) {
            Airfoil dragfoil = airfoil_template;
            skin_friction(waveAirfoil, dragfoil);
            airfoils.push_back(dragfoil);
        };

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
        double delta = alpha - seg.angle;   // Same formula, sign handles itself
        double Cp = ( 2.0 * delta ) / std::sqrt(M0*M0 - 1.0);
        double P  = P0 * (1.0 + ((gamma*M0*M0*Cp)/2));

        set_segment_state(seg, M0, P, T0, rho0);
    };
}

double Solver::iterare_recovery_factor(double r_guess, double s, Segment seg){

    double r = r_guess;
    double r_old = 0;
    double num_iterations = 0;

    double T_wall_adia;
    double T_reference;
    double dynamic_viscocity;
    double density;
    double Re_s;
    double Prandtl_number;

    while (fabs(r - r_old) > 1e-3 && num_iterations < 300) {
        T_wall_adia = seg.state.T * (1 + r * ((gamma - 1)/2) * seg.state.M * seg.state.M);
        T_reference = seg.state.T * (0.5 + 0.039 * seg.state.M * seg.state.M + 0.5 * (T_wall_adia/seg.state.T));
        dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
        density = seg.state.p / (287 * T_reference);
        Re_s = (density * (seg.state.M * std::sqrt(gamma * 287 * T_reference)) * s) / dynamic_viscocity;

        Prandtl_number = (1005 * dynamic_viscocity) / (0.0241 * std::pow(T_reference/273, 1.5) * ((273 + 194)/(T_reference + 194)));

        r_old = r;
        if (Re_s < 5e5) {
            r = sqrt(Prandtl_number);
        } else {
            r = std::pow(Prandtl_number, 1/3);
        };
        num_iterations += 1;
    };

    return r;
    
};

FrictionForces Solver::compute_surface_skin_friction(std::vector<Segment>& segs) {

    double panel_drag_coefficient;
    double drag_coefficient = 0;
    double lift_coefficient = 0; 
    double Normal_component;
    double Axial_compoenent;
    double panel_skin_friction_coefficient;
    double s = 0;
    double transition = true; 

    for (size_t i = 0; i < segs.size(); ++i){

        double panel_distance = segs[i].distance();
        double distance = s + panel_distance/2;
        double r = iterare_recovery_factor(0.9, distance, segs[i]);
        double T_wall_adia = segs[i].state.T * (1 + r * ((gamma - 1)/2) * segs[i].state.M * segs[i].state.M);
        double T_reference = segs[i].state.T * (0.5 + 0.039 * segs[i].state.M * segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
        double dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
        double density = segs[i].state.p / (287 * T_reference);
        double Re_s = (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)) * distance) / dynamic_viscocity;

        if (Re_s < 5e5) {
           
            panel_skin_friction_coefficient = 0.664 / std::sqrt(Re_s);
            panel_drag_coefficient = panel_skin_friction_coefficient * (panel_distance/1); // chord lenght is one

        } else {

            if (transition == true) {

                double s_crit = (5e5 * dynamic_viscocity) / (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)));

                if (s_crit <= s){

                    // switch must have happened at back end of panel or in between so ignore and asusme all turbulent. 

                    panel_skin_friction_coefficient = 0.0576 / std::pow(Re_s, 0.2);
                    panel_drag_coefficient = panel_skin_friction_coefficient * (panel_distance/1); // chord lenght is one

                } else {

                    // iterate to find s_crit, then find Reynolds numbers for the laminar and turbulent zone and then find ther coresponding skin friction and add them

                    double s_crit_old = 0;
                    double num_iterations = 0;

                    while (fabs(s_crit - s_crit_old) > 1e-3 && num_iterations < 200){

                        r = iterare_recovery_factor(0.9, s_crit, segs[i]);

                        T_wall_adia =  segs[i].state.T * (1 + r * ((gamma - 1)/2) *  segs[i].state.M *  segs[i].state.M);
                        T_reference =  segs[i].state.T * (0.5 + 0.039 *  segs[i].state.M *  segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
                        dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
                        density =  segs[i].state.p / (287 * T_reference);
                        
                        s_crit_old = s_crit;
                        s_crit = (5e5 * dynamic_viscocity) / (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)));

                        num_iterations += 1;
                    };

                    double d_laminar = s + (s_crit - s)/2;
                    double d_turbulent = s + (s + panel_distance - s_crit)/2;

                    r = iterare_recovery_factor(0.9, d_laminar, segs[i]);
                    T_wall_adia = segs[i].state.T * (1 + r * ((gamma - 1)/2) * segs[i].state.M * segs[i].state.M);
                    T_reference = segs[i].state.T * (0.5 + 0.039 * segs[i].state.M * segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
                    dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
                    density = segs[i].state.p / (287 * T_reference);
                    Re_s = (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)) * d_laminar) / dynamic_viscocity;

                    panel_skin_friction_coefficient = 0.664 / std::sqrt(Re_s);
                    panel_drag_coefficient += panel_skin_friction_coefficient * ((s_crit - s)/1); // chord lenght is one

                    r = iterare_recovery_factor(0.9, d_turbulent, segs[i]);
                    T_wall_adia = segs[i].state.T * (1 + r * ((gamma - 1)/2) * segs[i].state.M * segs[i].state.M);
                    T_reference = segs[i].state.T * (0.5 + 0.039 * segs[i].state.M * segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
                    dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
                    density = segs[i].state.p / (287 * T_reference);
                    Re_s = (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)) * d_turbulent) / dynamic_viscocity;

                    panel_skin_friction_coefficient = 0.0576 / std::pow(Re_s, 0.2);
                    panel_drag_coefficient = panel_skin_friction_coefficient * ((s + panel_distance - s_crit)/1); // chord lenght is one

                };

                transition = false;

            } else {
                
                panel_skin_friction_coefficient = 0.0576 / std::pow(Re_s, 0.2);
                panel_drag_coefficient = panel_skin_friction_coefficient * (panel_distance/1); // chord lenght is one

            }
        
        };

        Axial_compoenent = panel_drag_coefficient * cos(segs[i].angle);
        Normal_component = panel_drag_coefficient * sin(segs[i].angle);
        drag_coefficient += Normal_component * cos(alpha) - Axial_compoenent * sin(alpha);
        lift_coefficient += Normal_component * sin(alpha) + Axial_compoenent * cos(alpha);

        s += panel_distance;

    };

    return {drag_coefficient, lift_coefficient};

}

void Solver::skin_friction(Airfoil &airfoil, Airfoil &result_airfoil) {
    // needs to be fed airfoil from shock/expansion solver
    // Uses blassius solution, with compresisble adjustmeents using an adiabatic wall temperature to compute a refernece temperature
    // for the boundary layer and then computes the adjusted viscoty using sutherlands formula. 
    // Considers both laminar and tubrulent and trnaistional boundary layer states. 
    // Computes the skin friction across both top and bottom of airfoil
    FrictionForces top_friction = compute_surface_skin_friction(airfoil.top_segments);
    FrictionForces bottom_friction = compute_surface_skin_friction(airfoil.bottom_segments);

    result_airfoil.Forces.CL = top_friction.CL + bottom_friction.CL;
    result_airfoil.Forces.CL = top_friction.CD + bottom_friction.CD;
 
}


void Solver::set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho) {
    airfoil_segment.state.M = M;
    airfoil_segment.state.p = P;
    airfoil_segment.state.T = T;
    airfoil_segment.state.rho = rho;
};
