#pragma once
#include "Airfoil/airfoil.hpp"

#include <map>
#include <cmath>
#include <tuple>

constexpr double PI = 3.14159265358979323846;

struct PolarKey {
    std::string airfoil_hash;
    double M;
    double Re;
    bool visc;

    bool operator<(const PolarKey& other) const {
        return std::tie(airfoil_hash, M, Re) <
               std::tie(other.airfoil_hash, other.M, other.Re);
    }
};

struct PolarData {
    std::vector<double> AoA;
    std::vector<double> CL;
    std::vector<double> Cd;
    std::vector<double> Cm;
};

struct AeroData {
    double CL;
    double Cd;
    double Cm;
};

// add reynolds numebr to ui for visual sake
class XfoilWrapper {
    private:
    
    std::string input_file = "xfoil_run_file.txt";
    std::string polar_results_file = "xfoil_output.txt";
    std::string xfoil_log_file = "xfoil_log.txt";

    double iter_AoA = 0.1; //default AoA step size
    double lower_AoA = -5;
    double higher_AoA = 20;
    std::map<PolarKey, PolarData> polar_cache;

    PolarData run_xfoil(Airfoil airfoil, double M, double Re, bool visc);
    AeroData interpolate(PolarData key, double AoA);
    // interpolate_polar();

    public: // need to add this
    void set_iter(double AoA_step_size){
        AoA_step_size *= 180/PI;
        if (AoA_step_size < iter_AoA){
        iter_AoA = AoA_step_size;
        };
    };
    void set_lower(double AoA_lower_bound){
        AoA_lower_bound *= 180/PI;
        if (AoA_lower_bound < lower_AoA){
        lower_AoA = AoA_lower_bound;
        };
    };
    void set_higher(double AoA_higher_bound){
        AoA_higher_bound *= 180/PI;
        if (AoA_higher_bound > higher_AoA){
        higher_AoA = AoA_higher_bound;
        };
    };

    void solve(Airfoil &airfoil, double AoA, double M, double Re, bool visc);

    XfoilWrapper(){};

};