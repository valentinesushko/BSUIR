/**
 * @file polynomial_core.cpp
 * @brief Core implementation of the Polynomial class.
 */

#include "Polynomial.h"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

Polynomial::Polynomial() : degree_(0), coefficients_(1, 0.0) {}

Polynomial::Polynomial(int degree) : degree_(degree), coefficients_() {
    if (degree < 0) {
        throw std::invalid_argument("Polynomial degree must be non-negative");
    }

    coefficients_.assign(static_cast<std::size_t>(degree) + 1u, 0.0);
    normalize();
}

Polynomial::Polynomial(int degree, const double* coefficients)
    : degree_(degree), coefficients_() {
    if (degree < 0) {
        throw std::invalid_argument("Polynomial degree must be non-negative");
    }
    if (coefficients == nullptr) {
        throw std::invalid_argument("Polynomial coefficient pointer must not be null");
    }

    coefficients_.assign(coefficients,
                         coefficients + static_cast<std::size_t>(degree) + 1u);
    normalize();
}

Polynomial::Polynomial(std::initializer_list<double> coefficients)
    : degree_(-1), coefficients_(coefficients) {
    normalize();
}

Polynomial::Polynomial(const Polynomial& other)
    : degree_(other.degree_), coefficients_(other.coefficients_) {}

Polynomial& Polynomial::operator=(const Polynomial& other) {
    if (this != &other) {
        degree_ = other.degree_;
        coefficients_ = other.coefficients_;
    }
    return *this;
}

Polynomial::~Polynomial() = default;

int Polynomial::degree() const noexcept {
    return degree_;
}

double& Polynomial::operator[](int index) {
    if (index < 0 || index > degree_) {
        throw std::out_of_range("Polynomial coefficient index out of range");
    }
    return coefficients_[static_cast<std::size_t>(index)];
}

const double& Polynomial::operator[](int index) const {
    if (index < 0 || index > degree_) {
        throw std::out_of_range("Polynomial coefficient index out of range");
    }
    return coefficients_[static_cast<std::size_t>(index)];
}

double Polynomial::operator()(double x) const {
    double result = 0.0;
    for (std::size_t i = coefficients_.size(); i > 0; --i) {
        result = result * x + coefficients_[i - 1u];
    }
    return result;
}

void Polynomial::normalize() noexcept {
    while (coefficients_.size() > 1u && coefficients_.back() == 0.0) {
        coefficients_.pop_back();
    }

    if (coefficients_.empty()) {
        coefficients_.push_back(0.0);
    }

    degree_ = static_cast<int>(coefficients_.size()) - 1;
}

bool operator==(const Polynomial& lhs, const Polynomial& rhs) {
    return lhs.degree_ == rhs.degree_ &&
           lhs.coefficients_ == rhs.coefficients_;
}

bool operator!=(const Polynomial& lhs, const Polynomial& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Polynomial& polynomial) {
    os << polynomial.degree_;
    for (double coefficient : polynomial.coefficients_) {
        os << ' ' << coefficient;
    }
    return os;
}

std::istream& operator>>(std::istream& is, Polynomial& polynomial) {
    int degree = 0;
    if (!(is >> degree)) {
        return is;
    }

    if (degree < 0) {
        is.setstate(std::ios::failbit);
        return is;
    }

    const std::size_t count = static_cast<std::size_t>(degree) + 1u;

    try {
        std::vector<double> coefficients;
        coefficients.reserve(count);

        for (std::size_t i = 0; i < count; ++i) {
            double coefficient = 0.0;
            if (!(is >> coefficient)) {
                is.setstate(std::ios::failbit);
                return is;
            }
            coefficients.push_back(coefficient);
        }

        Polynomial parsed(degree, coefficients.data());
        polynomial = std::move(parsed);
    } catch (const std::exception&) {
        is.setstate(std::ios::failbit);
    }

    return is;
}
