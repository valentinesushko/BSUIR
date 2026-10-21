#include "Polynomial.h"

#include <stdexcept>
#include <utility>
#include <vector>

Polynomial& Polynomial::operator+=(const Polynomial& rhs)
{
    if (rhs.degree_ > degree_) {
        coefficients_.resize(static_cast<std::size_t>(rhs.degree_) + 1, 0.0);
        degree_ = rhs.degree_;
    }

    for (int i = 0; i <= rhs.degree_; ++i) {
        coefficients_[static_cast<std::size_t>(i)] +=
            rhs.coefficients_[static_cast<std::size_t>(i)];
    }

    normalize();
    return *this;
}

Polynomial& Polynomial::operator-=(const Polynomial& rhs)
{
    if (rhs.degree_ > degree_) {
        coefficients_.resize(static_cast<std::size_t>(rhs.degree_) + 1, 0.0);
        degree_ = rhs.degree_;
    }

    for (int i = 0; i <= rhs.degree_; ++i) {
        coefficients_[static_cast<std::size_t>(i)] -=
            rhs.coefficients_[static_cast<std::size_t>(i)];
    }

    normalize();
    return *this;
}

Polynomial& Polynomial::operator*=(const Polynomial& rhs)
{
    const int result_degree = degree_ + rhs.degree_;
    std::vector<double> result(static_cast<std::size_t>(result_degree) + 1, 0.0);

    for (int i = 0; i <= degree_; ++i) {
        for (int j = 0; j <= rhs.degree_; ++j) {
            result[static_cast<std::size_t>(i + j)] +=
                coefficients_[static_cast<std::size_t>(i)] *
                rhs.coefficients_[static_cast<std::size_t>(j)];
        }
    }

    coefficients_ = std::move(result);
    degree_ = result_degree;
    normalize();
    return *this;
}

Polynomial& Polynomial::operator/=(const Polynomial& divisor)
{
    if (divisor.degree_ == 0 &&
        divisor.coefficients_[0] == 0.0) {
        throw std::domain_error(
            "Polynomial division by the zero polynomial");
    }

    if (degree_ < divisor.degree_) {
        *this = Polynomial();
        return *this;
    }

    const int quotient_degree = degree_ - divisor.degree_;
    std::vector<double> quotient(
        static_cast<std::size_t>(quotient_degree) + 1, 0.0);
    std::vector<double> remainder = coefficients_;

    const double divisor_leading =
        divisor.coefficients_[static_cast<std::size_t>(divisor.degree_)];

    while (static_cast<int>(remainder.size()) - 1 >= divisor.degree_) {
        const int remainder_degree =
            static_cast<int>(remainder.size()) - 1;
        const int quotient_index = remainder_degree - divisor.degree_;
        const double factor =
            remainder[static_cast<std::size_t>(remainder_degree)] /
            divisor_leading;

        quotient[static_cast<std::size_t>(quotient_index)] = factor;

        for (int i = 0; i <= divisor.degree_; ++i) {
            remainder[static_cast<std::size_t>(quotient_index + i)] -=
                factor *
                divisor.coefficients_[static_cast<std::size_t>(i)];
        }
        remainder[static_cast<std::size_t>(remainder_degree)] = 0.0;

        while (!remainder.empty() && remainder.back() == 0.0) {
            remainder.pop_back();
        }
    }

    coefficients_ = std::move(quotient);
    degree_ = quotient_degree;
    normalize();
    return *this;
}

Polynomial operator+(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result(lhs);
    result += rhs;
    return result;
}

Polynomial operator-(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result(lhs);
    result -= rhs;
    return result;
}

Polynomial operator*(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result(lhs);
    result *= rhs;
    return result;
}

Polynomial operator/(const Polynomial& lhs, const Polynomial& rhs)
{
    Polynomial result(lhs);
    result /= rhs;
    return result;
}
