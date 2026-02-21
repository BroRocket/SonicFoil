#include "Analysis/hess_smith.hpp"
#include "Tools/matrix.hpp"
#include "Airfoil/airfoil.hpp"

#include <iostream>
#include <vector> // can maybe remove
#include <cmath>
#include <algorithm>


HessSmith::HessSmith(Airfoil &airfoil, double alpha, double M0, double T0) : airfoil_template(airfoil), M(M0), T(T0), V(M0*std::sqrt(1.4*287*T)), AoA(alpha), A_ij(airfoil.bottom_segments.size() + airfoil.top_segments.size() + 1, airfoil.bottom_segments.size() + airfoil.top_segments.size() + 1) {

    airfoil_segments = airfoil.bottom_segments;
    std::reverse(airfoil_segments.begin(), airfoil_segments.end());
    for (Segment &seg : airfoil.top_segments) {
        airfoil_segments.push_back(seg);
    };
    // issue is end of bottom panels is what I need is the start and end switched which is what this does
    for (size_t i = 0; i < airfoil.bottom_segments.size(); ++i) {
        std::swap(airfoil_segments[i].start, airfoil_segments[i].end);
    }

    double r_ij;
    double r_ij1;
    double beta_ij;
    double val;

    double r_1j;
    double r_1j1;
    double r_nj;
    double r_nj1;
    double beta_1j;
    double beta_nj;
    double arg;

    b_i.reserve(airfoil_segments.size() + 1);

    // airfoil_segments.size() = N but since cpp indexes at zero we can index over N by going < airofil_segments.size() 
    // due to cpp indexing aifoil.segments.size() reprensets the N+1 element and airfoil_segmenets.size() - 1 represents the N element

    for (size_t i = 0; i < airfoil_segments.size(); ++i) {

        for (size_t j = 0; j < airfoil_segments.size(); ++j) {

            r_ij = distance(airfoil_segments[j].start, airfoil_segments[i].midpoint);
            r_ij1 = distance(airfoil_segments[j].end, airfoil_segments[i].midpoint);
            if (i == j) {
                beta_ij = M_PI;
            } else {
                arg = (r_ij*r_ij + r_ij1*r_ij1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_ij*r_ij1);
                arg = std::clamp(arg, -1.0, 1.0);
                beta_ij = std::acos(arg);
            }
            val = std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij) + std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij;
            
            A_ij.set(j, i, val);
        }

        // Circulation column (j = N) // represents N+1 column 
        val = 0.0;
        for (size_t j = 0; j < airfoil_segments.size(); ++j) {
            r_ij = distance(airfoil_segments[j].start, airfoil_segments[i].midpoint);
            r_ij1 = distance(airfoil_segments[j].end, airfoil_segments[i].midpoint);
            if (i == j) {
                beta_ij = M_PI;
            } else {
                arg = (r_ij*r_ij + r_ij1*r_ij1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_ij*r_ij1);
                arg = std::clamp(arg, -1.0, 1.0);
                beta_ij = std::acos(arg);
            }
            val += std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij) - std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij;
        };

        A_ij.set(airfoil_segments.size(), i, val);
        b_i.push_back(2 * M_PI * V * std::sin(airfoil_segments[i].angle - AoA));

    };
    b_i.push_back(-2 * M_PI * V * (std::cos(airfoil_segments[0].angle - AoA) + std::cos(airfoil_segments[airfoil_segments.size() - 1].angle - AoA)));

    // Kutta Row (i = N) // represents N+1 row
    for (size_t j = 0; j < airfoil_segments.size(); ++j) {

        r_1j = distance(airfoil_segments[j].start, airfoil_segments[0].midpoint);
        r_1j1 = distance(airfoil_segments[j].end, airfoil_segments[0].midpoint);
        if (0 == j) {
            beta_1j = M_PI;
        } else {
            arg = (r_1j*r_1j + r_1j1*r_1j1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_1j*r_1j1);
            arg = std::clamp(arg, -1.0, 1.0);
            beta_1j = std::acos(arg);
        }
        r_nj = distance(airfoil_segments[j].start, airfoil_segments[airfoil_segments.size()-1].midpoint);
        r_nj1 = distance(airfoil_segments[j].end, airfoil_segments[airfoil_segments.size()-1].midpoint);
        if (airfoil_segments.size()-1 == j) {
            beta_nj = M_PI;
        } else {
            arg = (r_nj*r_nj + r_nj1*r_nj1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_nj*r_nj1);
            arg = std::clamp(arg, -1.0, 1.0);
            beta_nj = std::acos(arg);
        };
        val = (sin(airfoil_segments[0].angle - airfoil_segments[j].angle)*beta_1j - std::cos(airfoil_segments[0].angle - airfoil_segments[j].angle)*std::log(r_1j1/r_1j)) + (std::sin(airfoil_segments[airfoil_segments.size()-1].angle - airfoil_segments[j].angle)*beta_nj - std::cos(airfoil_segments[airfoil_segments.size()-1].angle - airfoil_segments[j].angle)*std::log(r_nj1/r_nj));
    
        A_ij.set(j, airfoil_segments.size(), val);

    }

    // Circulation term // (i == j = N) represents element at N+1, N+1
    val = 0.0;
    for (size_t j = 0; j < airfoil_segments.size(); ++j) {
        r_1j = distance(airfoil_segments[j].start, airfoil_segments[0].midpoint);
        r_1j1 = distance(airfoil_segments[j].end, airfoil_segments[0].midpoint);
        arg = (r_1j*r_1j + r_1j1*r_1j1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_1j*r_1j1);
        arg = std::clamp(arg, -1.0, 1.0);
        beta_1j = std::acos(arg);
        r_nj = distance(airfoil_segments[j].start, airfoil_segments[airfoil_segments.size()-1].midpoint);
        r_nj1 = distance(airfoil_segments[j].end, airfoil_segments[airfoil_segments.size()-1].midpoint);
        arg = (r_nj*r_nj + r_nj1*r_nj1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_nj*r_nj1);
        arg = std::clamp(arg, -1.0, 1.0);
        beta_nj = std::acos(arg);

        val += std::cos(airfoil_segments[0].angle - airfoil_segments[j].angle)*beta_1j + std::sin(airfoil_segments[0].angle - airfoil_segments[j].angle)*std::log(r_1j1/r_1j) + std::cos(airfoil_segments[airfoil_segments.size()-1].angle - airfoil_segments[j].angle)*beta_nj + std::sin(airfoil_segments[airfoil_segments.size()-1].angle - airfoil_segments[j].angle)*std::log(r_nj1/r_nj);
    }
    A_ij.set(airfoil_segments.size(), airfoil_segments.size(), val);

};

Airfoil HessSmith::solve(double P0, double T0, double rho0, double gamma, double R) {

    std::vector<double> x_i = solve_pivot(A_ij, b_i);

    v_ti.reserve(airfoil_segments.size());
    double circulation_val;
    double source_val;
    double r_ij;
    double r_ij1;
    double arg;
    double beta_ij;

    for (size_t i = 0; i < airfoil_segments.size(); ++i) {

        circulation_val = 0.0;
        source_val = 0.0;
        for (size_t j = 0; j < airfoil_segments.size(); ++j) {
            r_ij = distance(airfoil_segments[j].start, airfoil_segments[i].midpoint);
            r_ij1 = distance(airfoil_segments[j].end, airfoil_segments[i].midpoint);
            if (i == j) {
                beta_ij = M_PI;
            } else {
                arg = (r_ij*r_ij + r_ij1*r_ij1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_ij*r_ij1);
                arg = std::clamp(arg, -1.0, 1.0);
                beta_ij = std::acos(arg);
            }
            source_val += x_i[j] * (std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij - std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij));

            circulation_val += std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij + std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij);
            
        };

        v_ti.push_back(V*std::cos(airfoil_segments[i].angle - AoA) + (source_val/(2*M_PI)) + ((x_i[airfoil_segments.size()]*circulation_val)/(2*M_PI)));

    };

    // Get the lift from koutta joukowski
    double total_length = 0.0;
    for (Segment &seg : airfoil_segments) {
        total_length += seg.length;
    }
    double Gamma = x_i[airfoil_segments.size()] * total_length;
    
    kutta_Cl = (2 * Gamma) / V;

    // update airfoil;
    size_t i = 0;
    for (Segment &seg: airfoil_template.bottom_segments) {
        double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
        double T_i = T + ((gamma - 1)*((V*V - v_ti[i]*v_ti[i])/(2*gamma*R)));
        double M_i = v_ti[i]/(std::sqrt(gamma*R*T_i));
        double rho_i = rho0 * std::pow(((1 + ((gamma - 1)/2)*M*M)/(1 + ((gamma - 1)/2)*M_i*M_i)), (1/gamma));
        if (M > 0.3) {
            Cp_i = Cp_i / (std::sqrt(1 - M*M));
        };
        double p_i = P0 + 0.5*rho0*V*V*Cp_i;
        set_state(seg, M_i, p_i, rho_i, T_i);
        ++i;
    };
    for (Segment &seg: airfoil_template.top_segments) {
        double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
        double M_i = M*(v_ti[i]/V);
        if (M > 0.3) {
            Cp_i = Cp_i / (std::sqrt(1 - M*M));
        };
        double p_i = P0 + 0.5*rho0*V*V*Cp_i;
        set_state(seg, M_i, p_i, rho0, T);
        ++i;
    };

    return airfoil_template;
}

double HessSmith::distance(Cordinate start, Cordinate end) {

    return std::hypot(end.x - start.x, end.y - start.y);;

};

void HessSmith::set_state(Segment &seg, double M, double P, double rho, double T) {

    seg.state.M = M;
    seg.state.p = P;
    seg.state.rho = rho;
    seg.state.T = T;

};