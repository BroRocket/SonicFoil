#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Airfoil/airfoil.hpp"
#include "Solvers/solver.hpp"

#include <optional>

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
            .def_readonly("midpoint", &Segment::midpoint)
            .def_readonly("state", &Segment::state)
            .def_readonly("angle", &Segment::angle)
            .def_readonly("x_distance", &Segment::x_distance)
            .def_readonly("y_distance", &Segment::y_distance)
            .def_readonly("length", &Segment::length);

        pybind11::class_<Airfoil>(m, "Airfoil")
            .def(pybind11::init<const std::string &>())
            .def_readonly("name", &Airfoil::name)
            .def_readonly("Forces", &Airfoil::Forces)
            .def_readonly("top_segments", &Airfoil::top_segments)
            .def_readonly("bottom_segments", &Airfoil::bottom_segments)
            .def_readonly("cordinates", &Airfoil::cordinates);

        pybind11::class_<FrictionForces>(m, "FrictionForces")
            .def_readonly("CD", &FrictionForces::CD)
            .def_readonly("CL", &FrictionForces::CL);

        pybind11::class_<Result>(m, "Result")
            .def(pybind11::init<std::optional<Airfoil>, std::optional<Airfoil>, std::optional<Airfoil>, std::optional<Airfoil>, std::optional<Airfoil>, std::optional<Airfoil>>())
            .def_readonly("wave_solution", &Result::wave_solution)
            .def_readonly("ackeret_solution", &Result::ackeret_solution)
            .def_readonly("skin_friction_solution_supersonic", &Result::skin_friction_solution_supersonic)
            .def_readonly("panel_forces_solution", &Result::panel_forces_solution)
            .def_readonly("panel_kutta_solution", &Result::panel_kutta_solution)
            .def_readonly("skin_friction_solution_subsonic", &Result::skin_friction_solution_subsonic);

        pybind11::class_<Solver>(m, "Solver")
            .def(pybind11::init<Airfoil&,  double>(),
                    pybind11::arg("airfoil"),
                    pybind11::arg("gamma_") = 1.4
                )
            .def("solve_single", &Solver::solve_single,
                pybind11::arg("method"),
                pybind11::arg("AoA"),
                pybind11::arg("M0"),
                pybind11::arg("P0"),
                pybind11::arg("T0"),
                pybind11::arg("rho0"))
            .def("solve_range", &Solver::solve_range,
                pybind11::arg("method"),
                pybind11::arg("angles"),
                pybind11::arg("M0"),
                pybind11::arg("P0"),
                pybind11::arg("T0"),
                pybind11::arg("rho0"))
            .def_readonly("Results", &Solver::Results)
            .def_readonly("success", &Solver::success)
            .def_readonly("error_msg", &Solver::error_msg);

};