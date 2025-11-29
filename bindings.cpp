#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Airfoil/airfoil.hpp"
#include "Solvers/solver.hpp"


PYBIND11_MODULE(sonicfoil_backend, m) {

        pybind11::class_<Cordinate>(m, "Cordinate")
            .def(pybind11::init<double, double>())
            .def_readonly("x", &Cordinate::x)
            .def_readonly("y", &Cordinate::y);

        pybind11::class_<Conditions>(m, "Conditions")
            .def_readonly("M", &Conditions::M)
            .def_readonly("p", &Conditions::p)
            .def_readonly("T", &Conditions::T)
            .def_readonly("rho", &Conditions::rho);

        pybind11::class_<AerodynamicCoefficients>(m, "AerodynamicCoefficients")
            .def_readonly("CL", &AerodynamicCoefficients::CL)
            .def_readonly("Cd", &AerodynamicCoefficients::Cd)
            .def_readonly("CL_Cd", &AerodynamicCoefficients::CL_Cd)
            .def_readonly("C_MLE", &AerodynamicCoefficients::C_MLE)
            .def_readonly("x_cp", &AerodynamicCoefficients::x_cp)
            .def_readonly("y_cp", &AerodynamicCoefficients::y_cp);
 
        pybind11::class_<Segment>(m, "Segment")
            .def_readonly("start", &Segment::start)
            .def_readonly("end", &Segment::end)
            .def_readonly("state", &Segment::state)
            .def_readonly("angle", &Segment::angle)
            .def_readonly("x_distance", &Segment::x_distance)
            .def_readonly("y_distance", &Segment::y_distance)
            .def("distance", &Segment::distance);

        pybind11::class_<Airfoil>(m, "Airfoil")
            .def(pybind11::init<const std::string &>())
            .def_readonly("Forces", &Airfoil::Forces)
            .def_readonly("top_segments", &Airfoil::top_segments)
            .def_readonly("bottom_segments", &Airfoil::bottom_segments)
            .def_readonly("cordinates", &Airfoil::cordinates);

        pybind11::class_<FrictionForces>(m, "FrictionForces")
            .def_readonly("CD", &FrictionForces::CD)
            .def_readonly("CL", &FrictionForces::CL);

        pybind11::class_<Solver>(m, "Solver")
            .def(pybind11::init<Airfoil&, std::string, bool, double, double, double, double, double, double>(),
                    pybind11::arg("airfoil"),
                    pybind11::arg("method"),
                    pybind11::arg("skin_drag"),
                    pybind11::arg("AoA"),
                    pybind11::arg("M"),
                    pybind11::arg("p"),
                    pybind11::arg("T"),
                    pybind11::arg("rho"),
                    pybind11::arg("gamma_") = 1.4
                )
            .def_readonly("airfoil_solutions", &Solver::airfoils);

};