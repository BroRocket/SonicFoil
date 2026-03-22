#include "Analysis/hess_smith.hpp"
#include "Tools/matrix.hpp"
#include "Airfoil/airfoil.hpp"

#include <iostream>
#include <vector> // can maybe remove
#include <cmath>
#include <algorithm>


HessSmith::HessSmith(Airfoil &airfoil, double alpha, double M0, double T0, double gamma_) : airfoil_template(airfoil), M(M0), T(T0), V(M0*std::sqrt(1.4*287*T)), AoA(alpha), gamma(gamma_), A_ij(airfoil.bottom_segments.size() + airfoil.top_segments.size() + 1, airfoil.bottom_segments.size() + airfoil.top_segments.size() + 1) {

    // // have correct apanel angle definition now but still providing wrong ouput
    // airfoil_segments = airfoil.bottom_segments;
    // std::reverse(airfoil_segments.begin(), airfoil_segments.end());
    // for (size_t i = 0; i < airfoil.bottom_segments.size(); ++i) {
    //     std::swap(airfoil_segments[i].start, airfoil_segments[i].end);
    //     airfoil_segments[i].angle = airfoil_segments[i].get_angle(airfoil_segments[i].start, airfoil_segments[i].end);
    // }
    // for (Segment &seg : airfoil.top_segments) {
    //     airfoil_segments.push_back(seg);
    // };
    // int i=0;
    // for (Segment &seg : airfoil_segments) {
    //     std::cout << "Panel" << i << " (" << seg.start.x << " , " << seg.start.y << ") -> (" << seg.end.x << "," << seg.end.y << ")\n";
    //     std::cout << "Norm Vector: [" << seg.norm_vec.first << "," << seg.norm_vec.second << "] Tangential Vector: [" << seg.tang_vec.first << "," << seg.tang_vec.second << "]\n";
    //     std::cout << "Angle: " << seg.angle << " rad\n" ;
    //     ++i;
    // }

    // //issue is end of bottom panels is what I need is the start and end switched which is what this does
    
    // double r_ij;
    // double r_ij1;
    // double beta_ij;
    // double val;
    // double arg;

    // b_i.reserve(airfoil_segments.size() + 1);

    // std::vector<double> kutta_row_sums(airfoil_segments.size(), 0.0); // build vector collecting values for the kutta row

    // //Vortex holder
    // double vortex_val = 0.0;

    // // airfoil_segments.size() = N but since cpp indexes at zero we can index over N by going < airofil_segments.size() 
    // // due to cpp indexing aifoil.segments.size() reprensets the N+1 element and airfoil_segmenets.size() - 1 represents the N element

    // // i is down j is across in amtrix however matrix class is sturctured as x and y
    // for (size_t i = 0; i < airfoil_segments.size(); ++i) {

    //     double circulation_val = 0.0; // Ciruclation holder
    //     for (size_t j = 0; j < airfoil_segments.size(); ++j) {

    //         r_ij = distance(airfoil_segments[j].start, airfoil_segments[i].midpoint);
    //         r_ij1 = distance(airfoil_segments[j].end, airfoil_segments[i].midpoint);
    //         if (i == j) {
    //             beta_ij = PI;
    //         } else {
    //             arg = (r_ij*r_ij + r_ij1*r_ij1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_ij*r_ij1);
    //             arg = std::clamp(arg, -1.0, 1.0);
    //             beta_ij = std::acos(arg);
    //         }
    //         //std::cout << "(" << i << ", " << j << ")\nR_ij = " << r_ij << " R_ij+1 = " << r_ij1 << " B_ij = " << beta_ij << "\n";
    //         val = std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij) + std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij;
            
    //         A_ij.set(j, i, val);

    //         // lok into this being Pi for last panel
    //         // Circulation column (j = N) // represents N+1 column A_i_N+1
    //         circulation_val += std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij) - std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij;            

            
    //         if (i == 0 || i == airfoil_segments.size() - 1){
    //             // Kutta Row additions
    //             kutta_row_sums[j] += std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij - std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij);
    //             // VOrtex additons
    //             vortex_val += std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij + std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij);
    //         }
    //     }

    //     // Set Circulation column (j = N) // represents N+1 column 
    //     A_ij.set(airfoil_segments.size(), i, circulation_val);

    //     b_i.push_back(2 * PI * V * std::sin(airfoil_segments[i].angle - AoA)); // check this next (angles) if still not solved 

    // };

    // // Kutta Row (i = N) // represents N+1 column 
    // for (size_t j = 0; j < airfoil_segments.size(); ++j) {
    //     A_ij.set(j, airfoil_segments.size(), kutta_row_sums[j]);
    // };
    // // Vortex element (i=N, j=N) // represents A_N+1_N+1
    // A_ij.set(airfoil_segments.size(), airfoil_segments.size(), vortex_val);

    // // Vortex Value in b_i replresents N+1 element
    // b_i.push_back(-2 * PI * V * (std::cos(airfoil_segments[0].angle - AoA) + std::cos(airfoil_segments[airfoil_segments.size() - 1].angle - AoA)));


    // try different method from source
    // Prep points for other solving method 
    // std::vector<double> x;
    // std::vector<double> y; 
    for (int i = airfoil.bottom_segments.size() - 1; i >= 0; --i) {
        x.push_back(airfoil.bottom_segments[i].end.x);
        y.push_back(airfoil.bottom_segments[i].end.y);
    };
    for (Segment &seg : airfoil.top_segments) {
        x.push_back(seg.start.x);
        y.push_back(seg.start.y);
    };
    x.push_back(airfoil.top_segments[airfoil.bottom_segments.size() - 1].end.x);
    std::cout << "x:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < x.size(); ++i) {
        std::cout << x[i];
        if (i + 1 < x.size()) std::cout << ", ";
    }
    std::cout << " ]\n";
    y.push_back(airfoil.top_segments[airfoil.bottom_segments.size() - 1].end.y);
    std::cout << "y:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < y.size(); ++i) {
        std::cout << y[i];
        if (i + 1 < y.size()) std::cout << ", ";
    }
    std::cout << " ]\n";
    size_t npt = x.size() - 1;

    xi.reserve(npt);
    yi.reserve(npt);
    teta.reserve(npt);

    for (int i = 0; i < npt; ++i)
    {
        int m1 = i;
        int m2 = (i + 1) % npt;

        xi[i] = 0.5 * (x[m1] + x[m2]);
        yi[i] = 0.5 * (y[m1] + y[m2]);

        double ty = y[m2] - y[m1];
        double tx = x[m2] - x[m1];

        // ex = (1,0)
        double dot = tx;          // ex·t = tx
        double det = ty;          // ex x t = ty

        teta[i] = std::atan2(det, dot); //angle calce?? should compare them
    }
    std::cout << "x_mid:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < xi.size(); ++i) {
        std::cout << xi[i];
        if (i + 1 < xi.size()) std::cout << ", ";
    }
    std::cout << " ]\n";
    std::cout << "y_mid:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < yi.size(); ++i) {
        std::cout << yi[i];
        if (i + 1 < yi.size()) std::cout << ", ";
    }
    std::cout << " ]\n";
    std::cout << "angles:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < teta.size(); ++i) {
        std::cout << teta[i];
        if (i + 1 < teta.size()) std::cout << ", ";
    }
    std::cout << " ]\n";
    
    std::vector<std::vector<double>> A_mat(npt+1, std::vector<double>(npt+1, 0.0));
    std::vector<double> rhs(npt+1, 0.0);

    for (int i = 0; i < npt; ++i)
    {
        double p1 = teta[i];

        for (int j = 0; j < npt; ++j)
        {
            int n1 = j;
            int n2 = (j + 1) % npt;

            double dx1 = xi[i] - x[n1];
            double dy1 = yi[i] - y[n1];
            double dx2 = xi[i] - x[n2];
            double dy2 = yi[i] - y[n2];

            double rij  = std::sqrt(dx1*dx1 + dy1*dy1);
            double rij1 = std::sqrt(dx2*dx2 + dy2*dy2);

            double p2 = teta[j];

            double p3;
            if (i == j){
                p3 = PI_;
            } else {
                double det = dx1*dy2 - dx2*dy1;
                double dot = dx2*dx1 + dy2*dy1;
                p3 = std::atan2(det, dot); // is it the Bij calc?
            }

            double coeff_source = (1.0/(2.0*PI_)) * ( std::sin(p1-p2)*std::log(rij1/rij) + std::cos(p1-p2)*p3 );

            double coeff_vortex = (1.0/(2.0*PI_)) * ( std::cos(p1-p2)*std::log(rij1/rij) - std::sin(p1-p2)*p3 );

            A_mat[i][j] += coeff_source;
            A_mat[i][npt] += coeff_vortex;

            if (i == 0 || i == npt-1){
                A_mat[npt][j] +=
                    (1.0/(2.0*PI_)) * ( std::sin(p1-p2)*p3 - std::cos(p1-p2)*std::log(rij1/rij) );

                A_mat[npt][npt] += (1.0/(2.0*PI_)) *( std::sin(p1-p2)*std::log(rij1/rij) + std::cos(p1-p2)*p3 );
            }
        }

        rhs[i] = V * std::sin(teta[i] - AoA);
    }

    rhs[npt] = -V * ( std::cos(teta[0]-AoA) + std::cos(teta[npt-1]-AoA) );
    
    b_i = rhs;
    for (size_t i = 0; i <= npt; ++i) {
        for (size_t j = 0; j <= npt; ++j){
            A_ij.set(j, i, A_mat[i][j]);
        }
    }
    

};

Airfoil HessSmith::solve(double P0, double T0, double rho0) {

    // rewrite this seciton to use same calcs as used above for the other system
    std::cout << "A_ij:\n";
    A_ij.print();
    std::cout << "b_i:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < b_i.size(); ++i) {
        std::cout << b_i[i];
        if (i + 1 < b_i.size()) std::cout << ", ";
    }
    std::cout << " ]\n";

    std::vector<double> sol = solve_pivot(A_ij, b_i);
     std::cout << "sol:\n";
    std::cout << "[ ";
    for (size_t i = 0; i < sol.size(); ++i) {
        std::cout << sol[i];
        if (i + 1 < sol.size()) std::cout << ", ";
    }
    std::cout << " ]\n";

    v_ti.reserve(airfoil_segments.size());
    // double circulation_val;
    // double source_val;
    // double r_ij;
    // double r_ij1;
    // double arg;
    // double beta_ij;

    // double gamma = x_i[airfoil_segments.size()];
    // // elements of x_i up to N+1 are q_j

    // for (size_t i = 0; i < airfoil_segments.size(); ++i) {

    //     circulation_val = 0.0;
    //     source_val = 0.0;
    //     for (size_t j = 0; j < airfoil_segments.size(); ++j) {
    //         r_ij = distance(airfoil_segments[j].start, airfoil_segments[i].midpoint);
    //         r_ij1 = distance(airfoil_segments[j].end, airfoil_segments[i].midpoint);
    //         if (i == j) {
    //             beta_ij = PI;
    //         } else {
    //             arg = (r_ij*r_ij + r_ij1*r_ij1 - std::pow(airfoil_segments[j].length, 2)) / (2*r_ij*r_ij1);
    //             arg = std::clamp(arg, -1.0, 1.0);
    //             beta_ij = std::acos(arg);
    //         }
    //         source_val += x_i[j] * (std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij - std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij));

    //         circulation_val += std::cos(airfoil_segments[i].angle - airfoil_segments[j].angle)*beta_ij + std::sin(airfoil_segments[i].angle - airfoil_segments[j].angle)*std::log(r_ij1/r_ij);
            
    //     };

    //     v_ti.push_back(V*std::cos(airfoil_segments[i].angle - AoA) + (source_val/(2*PI)) + ((gamma*circulation_val)/(2*PI))); 

    // };

    // std::cout << " Velocities (trailing edge to leading edge to trailing edge, bottom to top):\n[ ";
    // for (size_t i = 0; i < v_ti.size(); ++i) {
    //     std::cout << v_ti[i];
    //     if (i + 1 < v_ti.size()) std::cout << ", ";
    // }
    // std::cout << " ]\n";

    // // Get the lift from koutta joukowski
    // double total_length = 0.0;
    // for (Segment &seg : airfoil_segments) {
    //     total_length += seg.length;
    // }

    // double Gamma = gamma * total_length;
    // std::cout << "Gamma = " << Gamma << "\n";
    // kutta_Cl = (2 * Gamma) / (V);

    // // update airfoil;
    // size_t i = 0;
    // for (Segment &seg: airfoil_template.bottom_segments) {
    //     double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
    //     std::cout << Cp_i << "\n";
    //     if (M > 0.25) {
    //         // apply Karman-Tsien compressibility corrections
    //         Cp_i = Cp_i / ((std::sqrt(1 - M*M)) + 0.5 * (((M*M)/(1 + std::sqrt(1 - M*M)))*Cp_i));
    //     };
    //     double p_i = P0 + 0.5 * rho0 * V * V * Cp_i;
    //     double M_i = std::sqrt(((std::pow(P0/p_i, (gamma - 1)/gamma) * (1 + ((gamma - 1)/2)*M*M)) - 1) * (2/(gamma - 1)));
    //     double T_i = T * std::pow(p_i/P0, (gamma - 1)/gamma);
    //     double rho_i = rho0 * std::pow(p_i/P0, 1/gamma);
    //     set_state(seg, M_i, p_i, rho_i, T_i);
    //     ++i;
    // };
    // for (Segment &seg: airfoil_template.top_segments) {
    //     double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
    //     std::cout << Cp_i << "\n";
    //     if (M > 0.3) {
    //         Cp_i = Cp_i / (std::sqrt(1 - M*M));
    //     };
    //     double p_i = P0 + 0.5 * rho0 * V * V * Cp_i;
    //     double M_i = std::sqrt(((std::pow(P0/p_i, (gamma - 1)/gamma) * (1 + ((gamma - 1)/2)*M*M)) - 1) * (2/(gamma - 1)));
    //     double T_i = T * std::pow(p_i/P0, (gamma - 1)/gamma);
    //     double rho_i = rho0 * std::pow(p_i/P0, 1/gamma);
    //     set_state(seg, M_i, p_i, rho_i, T_i);
    //     ++i;
    // };

    // return airfoil_template;
    size_t npt = b_i.size();
    std::vector<double> v(npt - 1), cp(npt - 1);

    double total_length = 0.0;

    for (int i = 0; i < npt; ++i)
    {
        double sum1 = 0.0;
        double sum2 = 0.0;

        int m1 = i;
        int m2 = (i + 1) % npt;

        double panelLength = std::sqrt( (x[m1]-x[m2])*(x[m1]-x[m2]) + (y[m1]-y[m2])*(y[m1]-y[m2]) );

        total_length += panelLength;

        double p1 = teta[i];

        for (int j = 0; j < npt; ++j)
        {
            int n1 = j;
            int n2 = (j + 1) % npt;

            double dx1 = xi[i] - x[n1];
            double dy1 = yi[i] - y[n1];
            double dx2 = xi[i] - x[n2];
            double dy2 = yi[i] - y[n2];

            double rij  = std::sqrt(dx1*dx1 + dy1*dy1);
            double rij1 = std::sqrt(dx2*dx2 + dy2*dy2);

            double p2 = teta[j];

            double p3;
            if (i == j)
                p3 = PI_;
            else
            {
                double det = dx1*dy2 - dx2*dy1;
                double dot = dx2*dx1 + dy2*dy1;
                p3 = std::atan2(det, dot);
            }

            sum1 += ( std::sin(p1-p2)*p3 - std::cos(p1-p2)*std::log(rij1/rij) ) * (sol[j]/(2.0*PI_));

            sum2 += ( std::sin(p1-p2)*std::log(rij1/rij) + std::cos(p1-p2)*p3 ) * (gamma/(2.0*PI_));
        }

        v[i]  = V * std::cos(teta[i]-AoA) + sum1 + sum2;
        cp[i] = 1.0 - (v[i]/V)*(v[i]/V);
    }

    v_ti = v;

    std::cout << " Velocities (trailing edge to leading edge to trailing edge, bottom to top):\n[ ";
    for (size_t i = 0; i < v_ti.size(); ++i) {
        std::cout << v_ti[i];
        if (i + 1 < v_ti.size()) std::cout << ", ";
    }
    std::cout << " ]\n";

    // Get the lift from koutta joukowski
    // double total_length = 0.0;
    // for (Segment &seg : airfoil_segments) {
    //     total_length += seg.length;
    // }

    kutta_Cl = 2.0 * gamma * total_length / V;

    // update airfoil;
    size_t i = 0;
    std::cout << "Cp_top: [";
    //velocities are trailing edge to front edge then back over the front
    for (int seg_i = airfoil_template.bottom_segments.size()-1; seg_i >= 0; --seg_i) {
        double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
        std::cout << Cp_i << ", ";
        if (M > 0.25) {
            // apply Karman-Tsien compressibility corrections
            Cp_i = Cp_i / ((std::sqrt(1 - M*M)) + 0.5 * (((M*M)/(1 + std::sqrt(1 - M*M)))*Cp_i));
        };
        double p_i = P0 + 0.5 * rho0 * V * V * Cp_i;
        double M_i = std::sqrt(((std::pow(P0/p_i, (gamma - 1)/gamma) * (1 + ((gamma - 1)/2)*M*M)) - 1) * (2/(gamma - 1)));
        double T_i = T * std::pow(p_i/P0, (gamma - 1)/gamma);
        double rho_i = rho0 * std::pow(p_i/P0, 1/gamma);
        set_state(airfoil_template.bottom_segments[seg_i], M_i, p_i, rho_i, T_i);
        ++i;
    };
    std::cout << "]\nCp_bot: [";
    for (Segment &seg: airfoil_template.top_segments) {
        double Cp_i = 1 - ((v_ti[i]/V)*(v_ti[i]/V));
        std::cout << Cp_i << ", ";
        if (M > 0.3) {
            Cp_i = Cp_i / (std::sqrt(1 - M*M));
        };
        double p_i = P0 + 0.5 * rho0 * V * V * Cp_i;
        double M_i = std::sqrt(((std::pow(P0/p_i, (gamma - 1)/gamma) * (1 + ((gamma - 1)/2)*M*M)) - 1) * (2/(gamma - 1)));
        double T_i = T * std::pow(p_i/P0, (gamma - 1)/gamma);
        double rho_i = rho0 * std::pow(p_i/P0, 1/gamma);
        set_state(seg, M_i, p_i, rho_i, T_i);
        ++i;
    };
    std::cout << "]";

    return airfoil_template;
    
}

/**
 * Compute distance/hypotnuse of right triange given starting point and end point. Inputs must be of Coordinate Class. 
 *
 * @param[in] start the starting point of the line
 * @param[in] end the end point of the line
 * @returns distance from start to end point.
 */
double HessSmith::distance(Cordinate start, Cordinate end) {

    return std::hypot(end.x - start.x, end.y - start.y);;

};

void HessSmith::set_state(Segment &seg, double M, double P, double rho, double T) {

    seg.state.M = M;
    seg.state.p = P;
    seg.state.rho = rho;
    seg.state.T = T;

};