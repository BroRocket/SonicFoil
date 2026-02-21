#include "Solvers/solver.hpp"
#include "Analysis/oblique_expansion.hpp"
#include "Analysis/oblique_shock.hpp"
#include "Solvers/aerodynamic_forces.hpp"
#include "Analysis/hess_smith.hpp"

#include <cmath>
#include <vector>

Solver::Solver(Airfoil &airfoil_template, double gamma_)
    : success(true), error_msg("N/A"), airfoil_template(airfoil_template), gamma(gamma_) {};

Result Solver::solve_single(std::string method, double AoA, double M0, double P0, double T0, double rho0){

    if (M0 < 1.0) {

        Airfoil HessSmithfoil = airfoil_template;   
        double kutta_Cl = HessSmith_method(HessSmithfoil, AoA, M0, P0, T0, rho0);
        AerodynamicForces(HessSmithfoil, AoA, P0, M0);
        
        if (method[1] == 'h') {
            if (method[1] == 'd') {
            // do skin friction calc for subsonic
            };
        } else if (method[1] == 'k') {
            if (method[1] == 'd') {
            // do skin friction calc for subsonic
            };
        }
        



    } else { // throw std::invalid_argument("Freestream Mach must be > 1.");
  
    if (method[0] == 'w') {
        Airfoil wavefoil = airfoil_template;
        //solve first as then the modified arifoil copy is stored in vector
        waveshock_method(wavefoil, AoA, M0, P0, T0, rho0);
        AerodynamicForces(wavefoil, AoA, P0, M0);
        //wavefoil.print_airfoil(); // Remove later

        // Then do friction calculations
        if (method[1] == 'd') {
            Airfoil dragfoil = airfoil_template;
            skin_friction_supersonic(wavefoil, dragfoil, AoA);
            return Result(wavefoil, std::nullopt, dragfoil);
        } else {
            return Result(wavefoil, std::nullopt, std::nullopt);
        }

        
    } else if (method[0] == 'a') {
        Airfoil ackeretfoil = airfoil_template;
        ackeret_method(ackeretfoil, AoA, M0, P0, T0, rho0);
        AerodynamicForces(ackeretfoil, AoA, P0, M0);

        return Result(std::nullopt, ackeretfoil, std::nullopt);

    } else if (method[0] == 'b') {
        Airfoil wavefoil = airfoil_template;
        //solve first as then the modified arifoil copy is stored in vector
        waveshock_method(wavefoil, AoA, M0, P0, T0, rho0);
        AerodynamicForces(wavefoil, AoA, P0, M0);

        Airfoil ackeretfoil = airfoil_template;
        ackeret_method(ackeretfoil, AoA, M0, P0, T0, rho0);
        AerodynamicForces(ackeretfoil, AoA, P0, M0);

        if (method[1] == 'd') {
            Airfoil dragfoil = airfoil_template;
            skin_friction_supersonic(wavefoil, dragfoil, AoA);
            return Result(wavefoil, ackeretfoil, dragfoil);
        } else {
            return Result(wavefoil, ackeretfoil, std::nullopt);
        }


        } else {
            throw std::invalid_argument("Invalid Method");
        };

    };
};

void Solver::solve_range(std::string method, const std::vector<double>& angles, double M0, double P0, double T0, double rho0) {
    
    //auto start = std::chrono::high_resolution_clock::now();
    success = true;
    error_msg = "N/A";
    Results.clear();

    try {
        for (int i = 0; i < angles.size(); i++) {
            Result r = solve_single(method, angles[i], M0, P0, T0, rho0);
            Results.push_back(r);
        }
    } catch (const std::runtime_error& e) {
        success = false;
        error_msg = e.what();
    } catch (const std::exception& e) {
        success = false;
        error_msg = e.what();
    } catch (const std::invalid_argument& e) {
        success = false;
        error_msg = e.what();
    } catch (...) {
        std::cerr << " Unkown Error " << std::endl;
    };
    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    // std::cout << "Solver time = " << elapsed.count() << " seconds\n";
};

void Solver::waveshock_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0) {
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

};

void Solver::ackeret_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0) {

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

double Solver::HessSmith_method(Airfoil &airfoil, double alpha, double M0, double P0, double T0, double rho0) {

    HessSmith panel_solver(airfoil, alpha, M0, T0);
    airfoil = panel_solver.solve(P0, T0, rho0);
    return panel_solver.kutta_Cl;
    // Need to add subsonic drag and then also add Kutt lift versus aerodynamic forces computed lift check difference
    // Need to find best way to return the result
};



// Supersonic Friction

double Solver::iterare_recovery_factor(double r_guess, double s, Segment& seg){

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
            r = cbrt(Prandtl_number); 
        };
        num_iterations += 1;
    };

    return r;
    
};

FrictionForces Solver::compute_surface_skin_friction_supersonic(std::vector<Segment>& segs, double alpha) {

    double panel_drag_coefficient;
    double drag_coefficient = 0;
    double lift_coefficient = 0; 
    double panel_skin_friction_coefficient;
    double s = 0;
    bool transition = true; 

    for (size_t i = 0; i < segs.size(); ++i){

        double panel_distance = segs[i].length;
        double distance = s + panel_distance;
        double r = iterare_recovery_factor(0.9, distance, segs[i]);
        double T_wall_adia = segs[i].state.T * (1 + r * ((gamma - 1)/2) * segs[i].state.M * segs[i].state.M);
        double T_reference = segs[i].state.T * (0.5 + 0.039 * segs[i].state.M * segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
        double dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
        double density = segs[i].state.p / (287 * T_reference);
        double Re_s = (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)) * distance) / dynamic_viscocity;

        // could chang eot chekc end of plate isn't over the trnaisiton this would be smarter
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
                    int num_iterations = 0;

                    while (fabs(s_crit - s_crit_old) > 1e-3 && num_iterations < 500){

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
                    panel_drag_coefficient = panel_skin_friction_coefficient * ((s_crit - s)/1); // chord lenght is one

                    r = iterare_recovery_factor(0.9, d_turbulent, segs[i]);
                    T_wall_adia = segs[i].state.T * (1 + r * ((gamma - 1)/2) * segs[i].state.M * segs[i].state.M);
                    T_reference = segs[i].state.T * (0.5 + 0.039 * segs[i].state.M * segs[i].state.M + 0.5 * (T_wall_adia/segs[i].state.T));
                    dynamic_viscocity = 1.716e-5 * std::pow(T_reference/273, 1.5) * ((273 + 111)/(T_reference + 111));
                    density = segs[i].state.p / (287 * T_reference);
                    Re_s = (density * (segs[i].state.M * std::sqrt(gamma * 287 * T_reference)) * d_turbulent) / dynamic_viscocity;

                    panel_skin_friction_coefficient = 0.0576 / std::pow(Re_s, 0.2);
                    panel_drag_coefficient += panel_skin_friction_coefficient * ((s + panel_distance - s_crit)/1); // chord lenght is one

                };

                transition = false;

            } else {
                
                panel_skin_friction_coefficient = 0.0576 / std::pow(Re_s, 0.2);
                panel_drag_coefficient = panel_skin_friction_coefficient * (panel_distance/1); // chord lenght is one

            }
        
        };

        double F_x_body = panel_drag_coefficient * cos(segs[i].angle);
        double F_y_body = panel_drag_coefficient * sin(segs[i].angle);

        drag_coefficient += F_x_body * cos(alpha) + F_y_body * sin(alpha);
        lift_coefficient += F_y_body * cos(alpha) - F_x_body * sin(alpha);

        s += panel_distance;

    };

    return {drag_coefficient, lift_coefficient};

}

void Solver::skin_friction_supersonic(Airfoil &airfoil, Airfoil &result_airfoil, double alpha) {
    // needs to be fed airfoil from shock/expansion solver
    // Uses blassius solution, with compresisble adjustmeents using an adiabatic wall temperature to compute a refernece temperature
    // for the boundary layer and then computes the adjusted viscoty using sutherlands formula. 
    // Considers both laminar and tubrulent and trnaistional boundary layer states. 
    // Computes the skin friction across both top and bottom of airfoil
    FrictionForces top_friction = compute_surface_skin_friction_supersonic(airfoil.top_segments, alpha);
    FrictionForces bottom_friction = compute_surface_skin_friction_supersonic(airfoil.bottom_segments, alpha);

    result_airfoil.Forces.CL = top_friction.CL + bottom_friction.CL;
    result_airfoil.Forces.Cd = top_friction.CD + bottom_friction.CD;
    result_airfoil.Forces.CL_Cd = result_airfoil.Forces.CL/result_airfoil.Forces.Cd;
 
}

// Subsonic Skin fricton drag
// simple method using flat plate approximation

FrictionForces Solver::compute_surface_skin_friction_subsonic(std::vector<Segment>& segs, double alpha, double rho_inf, double U_inf, double mu_inf)
{
    double drag_coefficient = 0.0;
    double lift_coefficient = 0.0;
    double s = 0.0;
    bool transition = true;

    for (size_t i = 0; i < segs.size(); ++i)
    {
        double panel_length = segs[i].length;
        double U_e = segs[i].state.Ue;   // tangential velocity from panel method
        double distance = s + panel_length;

        double Re_s = (rho_inf * U_e * distance) / mu_inf;

        double Cf;
        double panel_drag;

        if (Re_s < 5e5)
        {
            // Laminar
            Cf = 0.664 / std::sqrt(Re_s);
            panel_drag = Cf * panel_length;
        }
        else
        {
            if (transition)
            {
                double s_crit = (5e5 * mu_inf) / (rho_inf * U_e);

                if (s_crit <= s)
                {
                    // Fully turbulent
                    Cf = 0.0592 / std::pow(Re_s, 0.2);
                    panel_drag = Cf * panel_length;
                }
                else
                {
                    // Split laminar + turbulent region

                    double Re_lam = rho_inf * U_e * s_crit / mu_inf;
                    double Cf_lam = 0.664 / std::sqrt(Re_lam);

                    double Re_turb = rho_inf * U_e * distance / mu_inf;
                    double Cf_turb = 0.0592 / std::pow(Re_turb, 0.2);

                    panel_drag =
                        Cf_lam * (s_crit - s) +
                        Cf_turb * (distance - s_crit);
                }

                transition = false;
            }
            else
            {
                Cf = 0.0592 / std::pow(Re_s, 0.2);
                panel_drag = Cf * panel_length;
            }
        }

        // Transform to body frame
        double Fx = panel_drag * cos(segs[i].angle);
        double Fy = panel_drag * sin(segs[i].angle);

        drag_coefficient += Fx * cos(alpha) + Fy * sin(alpha);
        lift_coefficient += Fy * cos(alpha) - Fx * sin(alpha);

        s += panel_length;
    }

    return {drag_coefficient, lift_coefficient};
}



void Solver::set_segment_state(Segment &airfoil_segment, double M, double P, double T, double rho) {
    airfoil_segment.state.M = M;
    airfoil_segment.state.p = P;
    airfoil_segment.state.T = T;
    airfoil_segment.state.rho = rho;
};
