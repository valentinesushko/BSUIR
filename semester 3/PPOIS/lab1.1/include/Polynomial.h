/**
 * @file Polynomial.h
 * @brief Declaration of the Polynomial class for univariate polynomials.
 */

#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <initializer_list>
#include <iosfwd>
#include <vector>

/**
 * @class Polynomial
 * @brief Represents a polynomial in one variable.
 *
 * Coefficients are stored in a std::vector<double> where index 0 is the
 * constant term, index 1 is the coefficient of x, and so on. The degree is
 * therefore always equal to coefficients_.size() - 1.
 *
 * All construction and arithmetic operations normalize the result by
 * removing leading zero coefficients. The zero polynomial always has degree 0
 * and a single coefficient equal to 0.
 *
 * The class performs no rounding or tolerance-based comparison: coefficients
 * are stored and compared exactly as doubles.
 */
class Polynomial {
public:
    /**
     * @brief Constructs the zero polynomial.
     *
     * The resulting polynomial has degree 0 and a single coefficient equal
     * to 0.
     */
    Polynomial();

    /**
     * @brief Constructs the zero polynomial with the requested nominal degree.
     *
     * The coefficients are initialized to zero and then normalized, so the
     * resulting polynomial is the zero polynomial with degree 0 and a single
     * coefficient equal to 0.
     *
     * @param degree Nominal degree of the polynomial; must be non-negative.
     * @throws std::invalid_argument If degree is less than 0.
     */
    explicit Polynomial(int degree);

    /**
     * @brief Constructs a polynomial from a degree and a coefficient array.
     *
     * Copies degree + 1 coefficients from @p coefficients, where
     * coefficients[0] is the constant term. The result is normalized by
     * removing leading zero coefficients.
     *
     * @param degree Degree of the polynomial; must be non-negative.
     * @param coefficients Pointer to at least degree + 1 coefficients; must
     *        not be null when degree is non-negative.
     * @throws std::invalid_argument If degree is less than 0, or if
     *         @p coefficients is null while degree is non-negative.
     */
    Polynomial(int degree, const double* coefficients);

    /**
     * @brief Constructs a polynomial from an initializer list of coefficients.
     *
     * The first element is the constant term. An empty list produces the zero
     * polynomial. The result is normalized by removing leading zero
     * coefficients.
     *
     * @param coefficients Coefficients ordered from the constant term upwards.
     */
    Polynomial(std::initializer_list<double> coefficients);

    /**
     * @brief Copy constructor.
     *
     * @param other Polynomial to copy.
     */
    Polynomial(const Polynomial& other);

    /**
     * @brief Copy assignment operator.
     *
     * @param other Polynomial to copy.
     * @return Reference to this polynomial after assignment.
     */
    Polynomial& operator=(const Polynomial& other);

    /**
     * @brief Move constructor.
     *
     * Transfers the coefficients and degree from the source polynomial.
     */
    Polynomial(Polynomial&&) noexcept = default;

    /**
     * @brief Move assignment operator.
     *
     * @return Reference to this polynomial after assignment.
     */
    Polynomial& operator=(Polynomial&&) noexcept = default;

    /**
     * @brief Destructor.
     */
    ~Polynomial();

    /**
     * @brief Returns the degree of the polynomial.
     *
     * The zero polynomial has degree 0.
     *
     * @return The normalized degree, always non-negative.
     */
    int degree() const noexcept;

    /**
     * @brief Provides mutable access to a coefficient.
     *
     * @param index Coefficient index; 0 denotes the constant term.
     * @return Reference to the coefficient at @p index.
     * @throws std::out_of_range If @p index is outside [0, degree()].
     */
    double& operator[](int index);

    /**
     * @brief Provides read-only access to a coefficient.
     *
     * @param index Coefficient index; 0 denotes the constant term.
     * @return Const reference to the coefficient at @p index.
     * @throws std::out_of_range If @p index is outside [0, degree()].
     */
    const double& operator[](int index) const;

    /**
     * @brief Evaluates the polynomial at a given argument using Horner's method.
     *
     * @param x Value at which to evaluate the polynomial.
     * @return The value of the polynomial at @p x.
     */
    double operator()(double x) const;

    /**
     * @brief Adds another polynomial to this one.
     *
     * The result is normalized by removing leading zero coefficients.
     *
     * @param other Polynomial to add.
     * @return Reference to this polynomial after addition.
     */
    Polynomial& operator+=(const Polynomial& other);

    /**
     * @brief Subtracts another polynomial from this one.
     *
     * The result is normalized by removing leading zero coefficients.
     *
     * @param other Polynomial to subtract.
     * @return Reference to this polynomial after subtraction.
     */
    Polynomial& operator-=(const Polynomial& other);

    /**
     * @brief Multiplies this polynomial by another one.
     *
     * The result is normalized by removing leading zero coefficients.
     *
     * @param other Polynomial to multiply by.
     * @return Reference to this polynomial after multiplication.
     */
    Polynomial& operator*=(const Polynomial& other);

    /**
     * @brief Divides this polynomial by another one using truncated long division.
     *
     * Only the quotient is retained; the remainder is discarded. If the
     * dividend has a lower degree than the divisor, the result is the zero
     * polynomial. The result is normalized.
     *
     * @param other Divisor polynomial; must not be the zero polynomial.
     * @return Reference to this polynomial after division.
     * @throws std::domain_error If @p other is the zero polynomial.
     */
    Polynomial& operator/=(const Polynomial& other);

    /**
     * @brief Compares two polynomials for exact equality.
     *
     * Both operands are expected to be normalized; their degrees and
     * coefficients are compared exactly.
     *
     * @param lhs Left-hand polynomial.
     * @param rhs Right-hand polynomial.
     * @return True if both polynomials have the same degree and all
     *         coefficients are exactly equal; false otherwise.
     */
    friend bool operator==(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Compares two polynomials for inequality.
     *
     * @param lhs Left-hand polynomial.
     * @param rhs Right-hand polynomial.
     * @return True if the polynomials are not exactly equal; false otherwise.
     */
    friend bool operator!=(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Produces the sum of two polynomials.
     *
     * The result is normalized.
     *
     * @param lhs Left-hand polynomial.
     * @param rhs Right-hand polynomial.
     * @return Sum of @p lhs and @p rhs.
     */
    friend Polynomial operator+(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Produces the difference of two polynomials.
     *
     * The result is normalized.
     *
     * @param lhs Left-hand polynomial.
     * @param rhs Right-hand polynomial.
     * @return Difference @p lhs - @p rhs.
     */
    friend Polynomial operator-(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Produces the product of two polynomials.
     *
     * The result is normalized.
     *
     * @param lhs Left-hand polynomial.
     * @param rhs Right-hand polynomial.
     * @return Product of @p lhs and @p rhs.
     */
    friend Polynomial operator*(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Produces the quotient of two polynomials using truncated long division.
     *
     * The remainder is discarded. A lower-degree dividend yields the zero
     * polynomial. The result is normalized.
     *
     * @param lhs Dividend polynomial.
     * @param rhs Divisor polynomial; must not be the zero polynomial.
     * @return Quotient of @p lhs divided by @p rhs.
     * @throws std::domain_error If @p rhs is the zero polynomial.
     */
    friend Polynomial operator/(const Polynomial& lhs, const Polynomial& rhs);

    /**
     * @brief Writes a polynomial in the format "degree c0 c1 ... cdegree".
     *
     * Values are separated by single spaces. No label is written and no
     * trailing newline is emitted. For example, the zero polynomial is
     * written as "0 0".
     *
     * @param os Output stream.
     * @param polynomial Polynomial to write.
     * @return Reference to @p os.
     */
    friend std::ostream& operator<<(std::ostream& os, const Polynomial& polynomial);

    /**
     * @brief Reads a polynomial in the format "degree c0 c1 ... cdegree".
     *
     * The degree must be non-negative and exactly degree + 1 coefficients must
     * be available. Input is read into a temporary, validated, and normalized;
     * the target is modified only on success. On any parse or validation
     * failure, failbit is set on @p is and the target polynomial is left
     * unchanged.
     *
     * @param is Input stream.
     * @param polynomial Polynomial to receive the parsed value.
     * @return Reference to @p is.
     */
    friend std::istream& operator>>(std::istream& is, Polynomial& polynomial);

private:
    int degree_;
    std::vector<double> coefficients_;

    /**
     * @brief Removes leading zero coefficients and establishes the zero polynomial invariant.
     *
     * Ensures that the zero polynomial has degree 0 and a single coefficient
     * equal to 0, and that degree_ equals coefficients_.size() - 1.
     */
    void normalize() noexcept;
};

#endif
