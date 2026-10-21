#include "Polynomial.h"

#include <UnitTest++/UnitTest++.h>

#include <stdexcept>

TEST(AdditionSameDegree)
{
    const Polynomial a{1.0, 2.0, 3.0};
    const Polynomial b{4.0, 5.0, 6.0};

    const Polynomial sum = a + b;

    CHECK_EQUAL(2, sum.degree());
    CHECK_EQUAL(5.0, sum[0]);
    CHECK_EQUAL(7.0, sum[1]);
    CHECK_EQUAL(9.0, sum[2]);
}

TEST(AdditionDifferentDegrees)
{
    const Polynomial a{1.0, 2.0};
    const Polynomial b{3.0, 4.0, 5.0, 6.0};

    const Polynomial sum = a + b;

    CHECK_EQUAL(3, sum.degree());
    CHECK_EQUAL(4.0, sum[0]);
    CHECK_EQUAL(6.0, sum[1]);
    CHECK_EQUAL(5.0, sum[2]);
    CHECK_EQUAL(6.0, sum[3]);
}

TEST(AdditionAssignOperator)
{
    Polynomial p{1.0, 2.0};
    const Polynomial q{0.0, 3.0, 4.0};

    Polynomial& result = (p += q);

    CHECK(&result == &p);
    CHECK_EQUAL(2, p.degree());
    CHECK_EQUAL(1.0, p[0]);
    CHECK_EQUAL(5.0, p[1]);
    CHECK_EQUAL(4.0, p[2]);
}

TEST(ChainedAddition)
{
    const Polynomial sum =
        Polynomial{1.0, 2.0} + Polynomial{0.0, 1.0} + Polynomial{3.0};

    CHECK_EQUAL(1, sum.degree());
    CHECK_EQUAL(4.0, sum[0]);
    CHECK_EQUAL(3.0, sum[1]);
}

TEST(SubtractionSameDegree)
{
    const Polynomial a{5.0, 2.0, 7.0};
    const Polynomial b{1.0, 2.0, 3.0};

    const Polynomial difference = a - b;

    CHECK_EQUAL(2, difference.degree());
    CHECK_EQUAL(4.0, difference[0]);
    CHECK_EQUAL(0.0, difference[1]);
    CHECK_EQUAL(4.0, difference[2]);
}

TEST(SelfSubtractionYieldsZero)
{
    const Polynomial a{3.0, -2.0, 5.0};

    const Polynomial zero = a - a;

    CHECK_EQUAL(0, zero.degree());
    CHECK_EQUAL(0.0, zero[0]);
    CHECK(zero == Polynomial());
    CHECK_EQUAL(5.0, a[2]);
}

TEST(SubtractionDifferentDegrees)
{
    const Polynomial a{1.0, 2.0, 3.0};
    const Polynomial b{4.0, 5.0};

    const Polynomial difference = a - b;

    CHECK_EQUAL(2, difference.degree());
    CHECK_EQUAL(-3.0, difference[0]);
    CHECK_EQUAL(-3.0, difference[1]);
    CHECK_EQUAL(3.0, difference[2]);
}

TEST(SubtractionAssignOperator)
{
    Polynomial p{1.0, 2.0, 3.0};
    const Polynomial q{0.0, 1.0, 1.0};

    Polynomial& result = (p -= q);

    CHECK(&result == &p);
    CHECK_EQUAL(2, p.degree());
    CHECK_EQUAL(1.0, p[0]);
    CHECK_EQUAL(1.0, p[1]);
    CHECK_EQUAL(2.0, p[2]);
}

TEST(MultiplicationConstants)
{
    const Polynomial a{2.0, 3.0};
    const Polynomial b{4.0};

    const Polynomial product = a * b;

    CHECK_EQUAL(1, product.degree());
    CHECK_EQUAL(8.0, product[0]);
    CHECK_EQUAL(12.0, product[1]);

    const Polynomial scalar_product = Polynomial{2.0} * Polynomial{5.0};
    CHECK_EQUAL(0, scalar_product.degree());
    CHECK_EQUAL(10.0, scalar_product[0]);
}

TEST(MultiplicationLinear)
{
    const Polynomial a{1.0, 1.0};
    const Polynomial b{-1.0, 1.0};

    const Polynomial product = a * b;

    CHECK_EQUAL(2, product.degree());
    CHECK_EQUAL(-1.0, product[0]);
    CHECK_EQUAL(0.0, product[1]);
    CHECK_EQUAL(1.0, product[2]);
    CHECK_CLOSE(3.0, product(2.0), 1e-12);
}

TEST(MultiplicationQuadratics)
{
    const Polynomial a{1.0, 0.0, 1.0};
    const Polynomial b{-1.0, 0.0, 1.0};

    const Polynomial product = a * b;

    CHECK_EQUAL(4, product.degree());
    CHECK_EQUAL(-1.0, product[0]);
    CHECK_EQUAL(0.0, product[1]);
    CHECK_EQUAL(0.0, product[2]);
    CHECK_EQUAL(0.0, product[3]);
    CHECK_EQUAL(1.0, product[4]);
}

TEST(MultiplicationAssignOperator)
{
    Polynomial p{1.0, 2.0};
    const Polynomial q{1.0, -1.0};

    Polynomial& result = (p *= q);

    CHECK(&result == &p);
    CHECK_EQUAL(2, p.degree());
    CHECK_EQUAL(1.0, p[0]);
    CHECK_EQUAL(1.0, p[1]);
    CHECK_EQUAL(-2.0, p[2]);
}

TEST(MultiplicationByZeroPolynomial)
{
    const Polynomial a{1.0, 2.0, 3.0};
    const Polynomial zero;

    const Polynomial product = a * zero;

    CHECK_EQUAL(0, product.degree());
    CHECK_EQUAL(0.0, product[0]);
    CHECK(product == Polynomial());
    CHECK(zero * a == Polynomial());
}

TEST(MultiplicationByOneIdentity)
{
    const Polynomial a{2.0, -3.0, 4.0};
    const Polynomial one{1.0};

    CHECK(a * one == a);
    CHECK(one * a == a);
    CHECK_EQUAL(2, (a * one).degree());
}

TEST(ExactDivision)
{
    const Polynomial dividend{-1.0, 0.0, 0.0, 1.0};
    const Polynomial divisor{-1.0, 1.0};

    const Polynomial quotient = dividend / divisor;

    CHECK_EQUAL(2, quotient.degree());
    CHECK_EQUAL(1.0, quotient[0]);
    CHECK_EQUAL(1.0, quotient[1]);
    CHECK_EQUAL(1.0, quotient[2]);
    CHECK(quotient * divisor == dividend);
}

TEST(DivisionWithRemainderIsTruncated)
{
    const Polynomial dividend{1.0, 0.0, 1.0};
    const Polynomial divisor{1.0, 1.0};

    const Polynomial quotient = dividend / divisor;

    CHECK_EQUAL(1, quotient.degree());
    CHECK_EQUAL(-1.0, quotient[0]);
    CHECK_EQUAL(1.0, quotient[1]);
    CHECK(quotient * divisor != dividend);

    const Polynomial remainder = dividend - quotient * divisor;
    CHECK_EQUAL(0, remainder.degree());
    CHECK_CLOSE(2.0, remainder[0], 1e-12);
    CHECK(dividend == quotient * divisor + remainder);
}

TEST(DivisionLowerDegreeDividendYieldsZero)
{
    const Polynomial dividend{1.0, 2.0};
    const Polynomial divisor{1.0, 0.0, 1.0};

    const Polynomial quotient = dividend / divisor;

    CHECK_EQUAL(0, quotient.degree());
    CHECK_EQUAL(0.0, quotient[0]);
    CHECK(quotient == Polynomial());
}

TEST(DivisionAssignOperator)
{
    Polynomial p{-1.0, 0.0, 0.0, 1.0};
    const Polynomial divisor{-1.0, 1.0};

    Polynomial& result = (p /= divisor);

    CHECK(&result == &p);
    CHECK_EQUAL(2, p.degree());
    CHECK_EQUAL(1.0, p[0]);
    CHECK_EQUAL(1.0, p[1]);
    CHECK_EQUAL(1.0, p[2]);

    const Polynomial dividend{-1.0, 0.0, 0.0, 1.0};
    CHECK(p * divisor == dividend);
}

TEST(DivisionByZeroPolynomialThrows)
{
    const Polynomial dividend{1.0, 2.0, 3.0};
    Polynomial mutable_dividend{1.0, 2.0, 3.0};
    const Polynomial zero;

    CHECK_THROW(dividend / zero, std::domain_error);
    CHECK_THROW(mutable_dividend /= zero, std::domain_error);
    CHECK_THROW(zero / zero, std::domain_error);
}

TEST(NormalizationAfterCancellation)
{
    const Polynomial a{1.0, 2.0, 3.0};
    const Polynomial b{0.0, 0.0, -3.0};

    const Polynomial sum = a + b;
    CHECK_EQUAL(1, sum.degree());
    CHECK_EQUAL(1.0, sum[0]);
    CHECK_EQUAL(2.0, sum[1]);

    const Polynomial difference = a - Polynomial{0.0, 0.0, 3.0};
    CHECK_EQUAL(1, difference.degree());
    CHECK_EQUAL(1.0, difference[0]);
    CHECK_EQUAL(2.0, difference[1]);
}

TEST(AdditionThenSubtractionIdentity)
{
    const Polynomial a{1.0, -2.0, 3.0};
    const Polynomial b{4.0, 5.0};

    CHECK((a + b) - b == a);
    CHECK((a - b) + b == a);
}
