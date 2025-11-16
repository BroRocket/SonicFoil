#pragma once
#include <cmath>
#include <stdexcept>
#include <limits>

template<typename F>
double solve_bisection(F f, double a, double b, double tol = 1e-8, int max_iter = 1000) {
    double fa = f(a);
    double fb = f(b);
    if (std::isnan(fa) || std::isnan(fb)) throw std::runtime_error("f(a) or f(b) is NaN");
    if (fa * fb > 0) throw std::invalid_argument("f(a) and f(b) must have opposite signs for bisection");

    for (int i = 0; i < max_iter; ++i) {
        double c = 0.5 * (a + b);
        double fc = f(c);
        if (std::isnan(fc)) throw std::runtime_error("f(c) became NaN");
        if (std::abs(fc) < tol || (b - a) * 0.5 < tol) return c;
        if (fa * fc <= 0) { b = c; fb = fc; }
        else { a = c; fa = fc; }
    }
    return 0.5 * (a + b);
}