
#pragma once
#include <cstddef>
#include <utility>
#include <tuple>
#include <vector>

class Matrix {
    private:
    size_t n_col = 0;
    size_t m_row = 0;

    std::vector<double> mat;

    size_t index(size_t x, size_t y) const;

    public:

    std::pair<size_t, size_t> size() const;

    void set(size_t x, size_t y, double val);

    double get(size_t x, size_t y) const;

    void set_row(size_t y, const std::vector<double>& values);

    void set_column(size_t x, const std::vector<double>& values);

    void resize(size_t n_cols, size_t m_rows);

    Matrix multiply(const Matrix& other) const;

    Matrix transpose() const;
    
    Matrix(size_t n_cols, size_t m_rows, bool identity = false);
};

std::vector<double> solve(const Matrix& A, const std::vector<double> b);

std::vector<double> forwardsub(const Matrix& L, const std::vector<double>& b);

std::vector<double> backwardsub(const Matrix& U, const std::vector<double>& b);

std::pair<Matrix, Matrix> LU_factorization(const Matrix& A);

std::vector<double> solve_pivot(const Matrix& A, const std::vector<double>& b);

std::vector<double> backwardsub_pivot(const Matrix& U, const std::vector<double>& b);

std::vector<double> forwardsub_pivot(const Matrix& L, const std::vector<double>& b);

std::tuple<Matrix, Matrix, std::vector<size_t>> LU_factorization_pivot(const Matrix& A);

