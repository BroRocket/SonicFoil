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
    Segment(Cordinate start, Cordinate end) : start(start), end(end), angle(get_angle(start, end)), state(Conditions()), x_distance(std::abs(end.x - start.x)), y_distance(std::abs(end.y - start.y)) {};
    Cordinate start;
    Cordinate end;
    Conditions state;
    double angle; 
    double x_distance;
    double y_distance;

    double distance() {
        return std::sqrt(std::pow((end.x - start.x), 2) + std::pow((end.y -start.y), 2));
    };
};

class Airfoil {
private:
   
public:
    std::vector<Segment> top_segments;
    std::vector<Segment> bottom_segments; 
    std::vector<Cordinate> cordinates;
    std::string name; 
    // File is aranged trailing edge to leading edge and back around should sart and end with same point
    // should only have one zero zero point
    //Expects first line of fail to have airfoil name
    // SHould be in terms of chord length c
    Airfoil(const std::string& filename);

    
    void print_airfoil() {
        std::cout << "Top of Airfoil\n";
        for (size_t i = 0; i <top_segments.size(); i++) {
            std::cout << "Segment: Start ( " << top_segments[i].start.x << ", " << top_segments[i].start.y << " ) End: " << "( " << top_segments[i].end.x << ", " << top_segments[i].end.y << " ) Angle: " << top_segments[i].angle << " rad\n";
            std::cout << "Conditions: M = " << top_segments[i].state.M << " | p = " << top_segments[i].state.p << "Pa | T = " << top_segments[i].state.T << "K | rho = " << top_segments[i].state.rho << " kg/m^3\n";
        };
        std::cout << "Bottom of Airfoil\n";
        for (size_t i = 0; i <top_segments.size(); i++) {
            std::cout << "Segment: Start ( " << bottom_segments[i].start.x << ", " << bottom_segments[i].start.y << " ) End: " << "( " << bottom_segments[i].end.x << ", " << bottom_segments[i].end.y << " ) Angle: " << bottom_segments[i].angle << " rad\n";
            std::cout << "Conditions: M = " << bottom_segments[i].state.M << " | p = " << bottom_segments[i].state.p << "Pa | T = " << bottom_segments[i].state.T << "K | rho = " << bottom_segments[i].state.rho << " kg/m^3\n";
        };
    };

};