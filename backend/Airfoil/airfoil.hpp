#pragma once
#include <vector>
#include <string>
#include <cmath>

#include <iostream>

class Cordinate {
public:
    Cordinate(double x, double y) : x(x), y(y) {};
    double x, y;
};

class Conditions {
    public:
    Conditions() : M(0), p(0), T(0), rho(0) {};
    double M;
    double p;
    double T;
    double rho;
};

class Segment {
private: 
    double get_angle(Cordinate start, Cordinate end) {
        return std::atan2(end.y - start.y, end.x - start.x);
    }

public:
    Segment(Cordinate start, Cordinate end) : start(start), end(end), midpoint(Cordinate((start.x + end.x)/2, (start.y + end.y)/2)), state(Conditions()), angle(get_angle(start, end)), norm_vec({-std::sin(angle), std::cos(angle)}), tang_vec({std::cos(angle), std::sin(angle)}), x_distance(end.x - start.x), y_distance(end.y - start.y), length(std::hypot(x_distance, y_distance)) {};
    Cordinate start;
    Cordinate end;
    Cordinate midpoint;
    Conditions state;
    double angle; 
    std::pair<double, double> norm_vec;
    std::pair<double, double> tang_vec;
    double x_distance;
    double y_distance;
    double length;
};

class AerodynamicCoefficients{
    public:
    AerodynamicCoefficients() : CL(0), Cd(0), CL_Cd(0), C_MLE(0), x_cp(0), y_cp(0) {};
    double CL;
    double Cd;
    double CL_Cd;
    double C_MLE;
    double x_cp;
    double y_cp; 
};

class Airfoil {
private:
   
public:
    std::vector<Segment> top_segments;
    std::vector<Segment> bottom_segments; 
    std::vector<Cordinate> cordinates;
    std::string name; 
    AerodynamicCoefficients Forces;
    // File is aranged trailing edge to leading edge and back around should sart and end with same point
    // should only have one zero zero point
    //Expects first line of fail to have airfoil name
    // SHould be in terms of chord length c
    Airfoil(const std::string& filename);

    void print_airfoil() {
        std::cout << name << "\nTop of Airfoil\n";
        for (size_t i = 0; i <top_segments.size(); i++) {
            std::cout << "Segment: Start ( " << top_segments[i].start.x << ", " << top_segments[i].start.y << " ) End: " << "( " << top_segments[i].end.x << ", " << top_segments[i].end.y << " ) Angle: " << top_segments[i].angle << " rad\n";
            std::cout << "Conditions: M = " << top_segments[i].state.M << " | p = " << top_segments[i].state.p << "Pa | T = " << top_segments[i].state.T << "K | rho = " << top_segments[i].state.rho << " kg/m^3\n";
        };
        std::cout << "Bottom of Airfoil\n";
        for (size_t i = 0; i <bottom_segments.size(); i++) {
            std::cout << "Segment: Start ( " << bottom_segments[i].start.x << ", " << bottom_segments[i].start.y << " ) End: " << "( " << bottom_segments[i].end.x << ", " << bottom_segments[i].end.y << " ) Angle: " << bottom_segments[i].angle << " rad\n";
            std::cout << "Conditions: M = " << bottom_segments[i].state.M << " | p = " << bottom_segments[i].state.p << "Pa | T = " << bottom_segments[i].state.T << "K | rho = " << bottom_segments[i].state.rho << " kg/m^3\n";
        };
        std::cout << "Forces: CL = " << Forces.CL << " | Cd = " << Forces.Cd << " | CL/Cd = " << Forces.CL_Cd << " | Cm_LE = " << Forces.C_MLE << " | X_cp = " << Forces.x_cp << "c | Y_cp = " << Forces.y_cp << "c\n";
    };

};