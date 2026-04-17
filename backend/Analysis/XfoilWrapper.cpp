
#include "Analysis/XfoilWrapper.hpp"

#include <map>

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>

void XfoilWrapper::solve(Airfoil &airfoil, double AoA, double M, double Re, bool visc){
    // expects degrees no radian
    AoA *= 180/PI;
    PolarData data;
    PolarKey key = {airfoil.name, M, Re, visc};
    if (polar_cache.find(key) == polar_cache.end()){
        data = run_xfoil(airfoil, M, Re, visc);
        polar_cache[key] = data;

    } else {
        data = polar_cache[key];
    }

    /// need to expand the analysis if iter or low alfa or high alfa has changed 
    if (data.requested_lower > lower_AoA){
        data = run_xfoil(airfoil, M, Re, visc);
        polar_cache[key] = data;
    } else if (data.requested_upper < higher_AoA){
        data = run_xfoil(airfoil, M, Re, visc);
        polar_cache[key] = data;
    } else if (data.requested_step > iter_AoA){
        data = run_xfoil(airfoil, M, Re, visc);
        polar_cache[key] = data;
    }; 

    
    AeroData res = interpolate(data, AoA);
    airfoil.Forces.CL = res.CL;
    airfoil.Forces.Cd = res.Cd;
    airfoil.Forces.C_MLE = res.Cm;
    if (visc){
    airfoil.Forces.CL_Cd = res.CL/res.Cd;
    };
};


PolarData XfoilWrapper::run_xfoil(Airfoil airfoil, double M, double Re, bool visc){
    PolarData polar;

    if (std::filesystem::exists(polar_results_file)){
        std::filesystem::remove(polar_results_file);
        std::filesystem::remove(input_file);
        std::filesystem::remove(xfoil_log_file);
    }

    std::ofstream script(input_file);
    script << "PLOP\n";
    script << "G\n\n"; // disable graphics
    // need to update path for portability
    script << "LOAD " << "Airfoils\\" << airfoil.name << ".dat\n";
    if (visc){
        script << "PANE\n";
    }
    script << "OPER\n";
    if (visc){
        script << "VISC " << Re << "\n";
    };
    script << "MACH " << M << "\n";
    script << "ITER 200\n";
    script << "PACC\n";
    script << polar_results_file << "\n\n";
    script << "ASEQ " << lower_AoA << " " << higher_AoA << " " << iter_AoA << "\n";
    script << "QUIT\n";
    script.close();

    // need to change this for portability
    std::string command = "\"C:\\Users\\Brody Howard\\Documents\\GitHub\\SonicFoil\\Airfoils\\xfoil.exe\" < " + input_file + " > " + xfoil_log_file + " 2>nul";
    system(command.c_str());

    // ---- Parse polar output ----
    std::ifstream polar_stream(polar_results_file);
    std::string line;

    // skip header lines
    for (int i = 0; i < 12; ++i)
        std::getline(polar_stream, line);

    while (std::getline(polar_stream, line))
    {
        std::stringstream ss(line);

        double aoa, cl, cd, cdp, cm;

        ss >> aoa >> cl >> cd >> cdp >> cm;

        if (ss.fail()){continue;};

        polar.AoA.push_back(aoa);
        polar.CL.push_back(cl);
        polar.Cd.push_back(cd);
        polar.Cm.push_back(cm);
    }

    polar_stream.close();

    if (polar.AoA.empty()) {
        throw std::runtime_error("XFOIL failed to generate polar data");
    } else if (polar.AoA.size() < 3) {
        throw std::runtime_error("Insufficient XFOIL data returned");
    }

    polar.requested_lower = lower_AoA;
    polar.requested_upper = higher_AoA;
    polar.requested_step  = iter_AoA;  

    return polar;
};

AeroData XfoilWrapper::interpolate(PolarData data, double AoA){
    if (AoA < data.AoA.front()){
        // need to fix this as it fails at zero. 
        throw std::invalid_argument("XFoil failed to solve full solution, particularly at an angle of attack less than " + std::to_string(data.AoA.front()));
    };

    if (AoA > data.AoA.back()){
        throw std::invalid_argument("XFoil failed to solve full solution, particularly at an angle of attack greater than " + std::to_string(data.AoA.back()));
    };

    size_t low = 0;
    size_t high = 1;;
    for (size_t i = 0; i < data.AoA.size(); ++i){
        if (data.AoA[i] > AoA){
            low = i-1;
            high = i;
            break;
        };
    };
    
    double CL = data.CL[low] + (AoA - data.AoA[low])*((data.CL[high] - data.CL[low])/(data.AoA[high] - data.AoA[low]));
    double Cd = data.Cd[low] + (AoA - data.AoA[low])*((data.Cd[high] - data.Cd[low])/(data.AoA[high] - data.AoA[low]));
    double Cm = data.Cm[low] + (AoA - data.AoA[low])*((data.Cm[high] - data.Cm[low])/(data.AoA[high] - data.AoA[low]));

    return {CL, Cd, Cm};
};