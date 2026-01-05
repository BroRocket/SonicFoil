// matrix.cpp
#include "matrix.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <tuple>
#include <vector>
#include <cmath>

size_t Matrix::index(size_t x, size_t y) const{
    return (y * n_col) + x;
};

std::pair<size_t, size_t> Matrix::size() const{
    return {n_col, m_row};
};

void Matrix::set(size_t x, size_t y, double val){
    if (x >= n_col){
        throw std::out_of_range("X cannot be greater then number of columns");
    } else if (y >= m_row){
        throw std::out_of_range("Y cannot be greater then number of rows");
    };
    mat[index(x, y)] = val;
};

double Matrix::get(size_t x, size_t y) const{
    if (x >= n_col){
        throw std::out_of_range("X cannot be greater then number of columns");
    } else if (y >= m_row){
        throw std::out_of_range("Y cannot be greater then number of rows");
    };
    return mat[index(x, y)];
};

void Matrix::set_row(size_t y, const std::vector<double>& values){
    if (y >= m_row){
        throw std::out_of_range("Y cannot be greater then number of rows");
    } else if (values.size() != n_col){
        throw std::invalid_argument("Row size must equal number of columns");
    };

    const size_t base = y * n_col;
    for (size_t x = 0; x < n_col; ++x){
        mat[base + x] = values[x];
    };
};

void Matrix::set_column(size_t x, const std::vector<double>& values){
    if (x >= n_col){
        throw std::out_of_range("X cannot be greater then number of columns");
    } else if (values.size() != m_row){
        throw std::invalid_argument("Column size must equal number of rows");
    };

    for (size_t y = 0; y < m_row; ++y){
        mat[index(x, y)] = values[y];
    };
};

void Matrix::resize(size_t n_cols, size_t m_rows){
    std::vector<double> new_mat(m_rows * n_cols, 0.0);

    const size_t copy_rows = std::min(m_row, m_rows);
    const size_t copy_cols = std::min(n_col, n_cols);

    for (size_t y = 0; y < copy_rows; ++y){
        const size_t old_base = y * n_col;
        const size_t new_base = y * n_cols;
        for (size_t x = 0; x < copy_cols; ++x){
            new_mat[new_base + x] = mat[old_base + x];
        };
    };

    mat = std::move(new_mat);
    n_col = n_cols;
    m_row = m_rows;
};

Matrix Matrix::multiply(const Matrix& other) const{
    if (n_col != other.m_row){
        throw std::invalid_argument("Matrix dimension mismatch for multiplication");
    };

    Matrix result(other.n_col, m_row, false);

    for (size_t y = 0; y < m_row; ++y){
        const size_t a_base = y * n_col;
        const size_t c_base = y * result.n_col;

        for (size_t k = 0; k < n_col; ++k){
            const double a_val = mat[a_base + k];
            const size_t b_base = k * other.n_col;

            for (size_t x = 0; x < other.n_col; ++x){
                result.mat[c_base + x] += a_val * other.mat[b_base + x];
            };
        };
    };

    return result;
};

Matrix Matrix::transpose() const{
    Matrix result(m_row, n_col, false);

    for (size_t y = 0; y < m_row; ++y){
        const size_t src_base = y * n_col;
        for (size_t x = 0; x < n_col; ++x){
            result.mat[result.index(y, x)] = mat[src_base + x];
        };
    };

    return result;
};

Matrix::Matrix(size_t n_cols, size_t m_rows, bool identity)
    : n_col(n_cols),
      m_row(m_rows),
      mat(m_rows * n_cols, 0.0) {
    if (identity){
        if (n_col != m_row){
            throw std::invalid_argument("Identity matrix requires rows == cols.");
        };
        for (size_t i = 0; i < m_row; ++i){
            mat[index(i, i)] = 1.0;
        };
    };
};

std::vector<double> forwardsub(const Matrix& L, const std::vector<double>& b){
    auto [n_cols, m_rows] = L.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("Forward substitution requires a square matrix");
    } else if (b.size() != m_rows){
        throw std::invalid_argument("Vector size must match number of rows");
    };

    std::vector<double> x(m_rows, 0.0);
    double temp;

    for (size_t i = 0; i < m_rows; ++i){
        temp = b[i];

        for (size_t j = 0; j < i; ++j){
            temp = temp - L.get(j, i) * x[j];
        };

        const double diag = L.get(i, i);
        if (diag == 0.0){
            throw std::runtime_error("Zero diagonal encountered in forward substitution");
        };

        x[i] = temp / diag;
    };

    return x;
};

std::vector<double> backwardsub(const Matrix& U, const std::vector<double>& b){
    auto [n_cols, m_rows] = U.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("Backward substitution requires a square matrix");
    } else if (b.size() != m_rows){
        throw std::invalid_argument("Vector size must match number of rows");
    };

    std::vector<double> x(m_rows, 0.0);
    double temp;

    for (size_t ii = m_rows; ii-- > 0; ){
        const size_t i = ii;
        temp = b[i];

        for (size_t j = i + 1; j < n_cols; ++j){
            temp = temp - U.get(j, i) * x[j];
        };

        const double diag = U.get(i, i);
        if (diag == 0.0){
            throw std::runtime_error("Zero diagonal encountered in backward substitution");
        };

        x[i] = temp / diag;
    };

    return x;
};

std::pair<Matrix, Matrix> LU_factorization(const Matrix& A){
    auto [n_cols, m_rows] = A.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("LU factorization requires a square matrix");
    } else if (n_cols == 0){
        throw std::invalid_argument("LU factorization requires a non-empty matrix");
    };

    Matrix L(n_cols, n_cols, true);
    Matrix U = A;

    for (size_t i = 0; i + 1 < n_cols; ++i){
        const double pivot = U.get(i, i);
        if (pivot == 0.0){
            throw std::runtime_error("Zero pivot encountered in LU factorization (no pivoting)");
        };

        for (size_t j = i + 1; j < n_cols; ++j){
            const double factor = U.get(i, j) / pivot;

            L.set(i, j, factor);

            for (size_t k = i; k < n_cols; ++k){
                U.set(k, j, U.get(k, j) - factor * U.get(k, i));
            };
        };
    };

    return {L, U};
};

std::vector<double> solve(const Matrix& A, const std::vector<double> b){
    auto [L, U] = LU_factorization(A);
    std::vector<double> z = forwardsub(L, b);
    std::vector<double> x = backwardsub(U, z);
    return x;
};

static void swap_rows(Matrix& M, size_t row_a, size_t row_b, size_t n_cols){
    if (row_a == row_b){
        return;
    };
    for (size_t x = 0; x < n_cols; ++x){
        const double tmp = M.get(x, row_a);
        M.set(x, row_a, M.get(x, row_b));
        M.set(x, row_b, tmp);
    };
};

std::vector<double> forwardsub_pivot(const Matrix& L, const std::vector<double>& b){
    auto [n_cols, m_rows] = L.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("Forward substitution requires a square matrix");
    } else if (b.size() != m_rows){
        throw std::invalid_argument("Vector size must match number of rows");
    };

    const double eps = 1e-12;

    std::vector<double> x(m_rows, 0.0);
    double temp;

    for (size_t i = 0; i < m_rows; ++i){
        temp = b[i];

        for (size_t j = 0; j < i; ++j){
            temp = temp - L.get(j, i) * x[j];
        };

        const double diag = L.get(i, i);
        if (std::abs(diag) <= eps){
            throw std::runtime_error("Near-zero diagonal encountered in forward substitution");
        };

        x[i] = temp / diag;
    };

    return x;
};

std::vector<double> backwardsub_pivot(const Matrix& U, const std::vector<double>& b){
    auto [n_cols, m_rows] = U.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("Backward substitution requires a square matrix");
    } else if (b.size() != m_rows){
        throw std::invalid_argument("Vector size must match number of rows");
    };

    const double eps = 1e-12;

    std::vector<double> x(m_rows, 0.0);
    double temp;

    for (size_t ii = m_rows; ii-- > 0; ){
        const size_t i = ii;
        temp = b[i];

        for (size_t j = i + 1; j < n_cols; ++j){
            temp = temp - U.get(j, i) * x[j];
        };

        const double diag = U.get(i, i);
        if (std::abs(diag) <= eps){
            throw std::runtime_error("Near-zero diagonal encountered in backward substitution");
        };

        x[i] = temp / diag;
    };

    return x;
};

std::tuple<Matrix, Matrix, std::vector<size_t>> LU_factorization_pivot(const Matrix& A){
    auto [n_cols, m_rows] = A.size();

    if (n_cols != m_rows){
        throw std::invalid_argument("LU factorization requires a square matrix");
    } else if (n_cols == 0){
        throw std::invalid_argument("LU factorization requires a non-empty matrix");
    };

    const double eps = 1e-12;
    const size_t n = n_cols;

    Matrix L(n, n, true);
    Matrix U = A;

    std::vector<size_t> piv(n, 0);
    for (size_t i = 0; i < n; ++i){
        piv[i] = i;
    };

    for (size_t i = 0; i + 1 < n; ++i){
        size_t pivot_row = i;
        double max_abs = std::abs(U.get(i, i));

        for (size_t r = i + 1; r < n; ++r){
            const double v = std::abs(U.get(i, r));
            if (v > max_abs){
                max_abs = v;
                pivot_row = r;
            };
        };

        if (max_abs <= eps){
            throw std::runtime_error("Singular or near-singular matrix encountered in LU factorization (pivoting)");
        };

        if (pivot_row != i){
            swap_rows(U, i, pivot_row, n);

            for (size_t k = 0; k < i; ++k){
                const double tmp = L.get(k, i);
                L.set(k, i, L.get(k, pivot_row));
                L.set(k, pivot_row, tmp);
            };

            std::swap(piv[i], piv[pivot_row]);
        };

        const double pivot = U.get(i, i);
        if (std::abs(pivot) <= eps){
            throw std::runtime_error("Zero or near-zero pivot encountered in LU factorization (pivoting)");
        };

        for (size_t j = i + 1; j < n; ++j){
            const double factor = U.get(i, j) / pivot;

            L.set(i, j, factor);

            for (size_t k = i; k < n; ++k){
                U.set(k, j, U.get(k, j) - factor * U.get(k, i));
            };
        };
    };

    if (std::abs(U.get(n - 1, n - 1)) <= eps){
        throw std::runtime_error("Singular or near-singular matrix encountered in LU factorization (pivoting)");
    };

    return {L, U, piv};
};

std::vector<double> solve_pivot(const Matrix& A, const std::vector<double>& b){
    auto [L, U, piv] = LU_factorization_pivot(A);

    if (b.size() != piv.size()){
        throw std::invalid_argument("Vector size must match number of rows");
    };

    std::vector<double> pb(b.size(), 0.0);
    for (size_t i = 0; i < piv.size(); ++i){
        pb[i] = b[piv[i]];
    };

    std::vector<double> z = forwardsub_pivot(L, pb);
    std::vector<double> x = backwardsub_pivot(U, z);
    return x;
};