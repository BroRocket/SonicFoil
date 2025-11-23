#include <airfoil.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <iostream>


Airfoil::Airfoil(const std::string& filename) : Forces(AerodynamicCoefficients()) {
    std::ifstream file(filename);
    std::string line; 
    bool top = true;


    std::getline(file, line);
    name = line;

    double x, y;
    while (std::getline(file, line)){
        std::istringstream iss(line);
        if (iss >> x >> y){
            cordinates.push_back(Cordinate(x, y));
        };
    };

    // sort into segmetns.
    // Need to sort points so they are in order first 

    for (size_t i = 0; i < cordinates.size() - 1; i++) {
        if (top == true) {
            top_segments.push_back(Segment(cordinates[i+1], cordinates[i]));
            if (cordinates[i+1].x == 0 || cordinates[i+1].y == 0) {
                top = false;
            };
        } else {    
            bottom_segments.push_back(Segment(cordinates[i], cordinates[i+1]));
        };
    };

    // invert top segement due to file format
    std::reverse(top_segments.begin(), top_segments.end());
        
}; 