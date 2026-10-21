#include "Polynomial.h"

#include <UnitTest++/UnitTest++.h>

#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
const double kTolerance = 1e-12;
}

TEST(DefaultConstructorCreatesZeroPolynomial)
{
    Polynomial polynomial;

    CHECK_EQUAL(0, polynomial.degree());
    CHECK_CLOSE(0.0, polynomial[0], kTolerance);
}

TEST(DegreeConstructorCreatesZeroPolynomial)
{
    Polynomial zeroDegree(0);
    Polynomial higherDegree(7);

    CHECK_EQUAL(0, zeroDegree.degree());
    CHECK_CLOSE(0.0, zeroDegree[0], kTolerance);
    CHECK_EQUAL(0, higherDegree.degree());
    CHECK_CLOSE(0.0, higherDegree[0], kTolerance);
}

TEST(DegreeConstructorThrowsOnNegativeDegree)
{
    CHECK_THROW(Polynomial(-1), std::invalid_argument);
}

TEST(PointerConstructorCopiesCoefficients)
{
    const double coefficients[] = {4.0, -3.0, 2.0};
    const Polynomial polynomial(2, coefficients);

    CHECK_EQUAL(2, polynomial.degree());
    CHECK_CLOSE(4.0, polynomial[0], kTolerance);
    CHECK_CLOSE(-3.0, polynomial[1], kTolerance);
    CHECK_CLOSE(2.0, polynomial[2], kTolerance);
}

TEST(PointerConstructorNormalizesLeadingZeros)
{
    const double coefficients[] = {1.5, 2.5, 0.0, 0.0};
    const Polynomial polynomial(3, coefficients);

    CHECK_EQUAL(1, polynomial.degree());
    CHECK_CLOSE(1.5, polynomial[0], kTolerance);
    CHECK_CLOSE(2.5, polynomial[1], kTolerance);
    CHECK_THROW(polynomial[2], std::out_of_range);
}

TEST(PointerConstructorThrowsOnNegativeDegree)
{
    const double coefficients[] = {1.0};
    CHECK_THROW(Polynomial(-1, coefficients), std::invalid_argument);
}

TEST(PointerConstructorThrowsOnNullCoefficients)
{
    const double* nullCoefficients = nullptr;

    CHECK_THROW(Polynomial(0, nullCoefficients), std::invalid_argument);
    CHECK_THROW(Polynomial(3, nullptr), std::invalid_argument);
}

TEST(InitializerListConstructorBuildsPolynomial)
{
    const Polynomial polynomial{4.0, -3.0, 2.0, 0.5};

    CHECK_EQUAL(3, polynomial.degree());
    CHECK_CLOSE(4.0, polynomial[0], kTolerance);
    CHECK_CLOSE(-3.0, polynomial[1], kTolerance);
    CHECK_CLOSE(2.0, polynomial[2], kTolerance);
    CHECK_CLOSE(0.5, polynomial[3], kTolerance);
}

TEST(InitializerListConstructorAcceptsEmptyList)
{
    const std::initializer_list<double> empty;
    const Polynomial polynomial(empty);

    CHECK_EQUAL(0, polynomial.degree());
    CHECK_CLOSE(0.0, polynomial[0], kTolerance);
}

TEST(InitializerListConstructorNormalizesLeadingZeros)
{
    const Polynomial padded{1.0, 2.0, 0.0, 0.0};
    const Polynomial compact{1.0, 2.0};

    CHECK_EQUAL(1, padded.degree());
    CHECK(padded == compact);
}

TEST(NonConstSubscriptAllowsCoefficientModification)
{
    Polynomial polynomial{1.0, 2.0, 3.0};

    polynomial[0] = 10.0;
    polynomial[2] = -7.5;

    CHECK_CLOSE(10.0, polynomial[0], kTolerance);
    CHECK_CLOSE(2.0, polynomial[1], kTolerance);
    CHECK_CLOSE(-7.5, polynomial[2], kTolerance);
}

TEST(ConstSubscriptProvidesReadOnlyAccess)
{
    const Polynomial polynomial{1.0, 2.0, 3.0};

    CHECK_CLOSE(1.0, polynomial[0], kTolerance);
    CHECK_CLOSE(2.0, polynomial[1], kTolerance);
    CHECK_CLOSE(3.0, polynomial[2], kTolerance);
}

TEST(SubscriptThrowsWhenIndexIsNegative)
{
    Polynomial polynomial{1.0, 2.0};
    const Polynomial constantPolynomial{1.0, 2.0};

    CHECK_THROW(polynomial[-1], std::out_of_range);
    CHECK_THROW(constantPolynomial[-1], std::out_of_range);
}

TEST(SubscriptThrowsWhenIndexExceedsDegree)
{
    Polynomial polynomial{1.0, 2.0};
    const Polynomial constantPolynomial{1.0, 2.0};

    CHECK_THROW(polynomial[2], std::out_of_range);
    CHECK_THROW(constantPolynomial[2], std::out_of_range);
}

TEST(EvaluatesConstantPolynomial)
{
    const Polynomial polynomial{5.0};

    CHECK_CLOSE(5.0, polynomial(0.0), kTolerance);
    CHECK_CLOSE(5.0, polynomial(-3.0), kTolerance);
    CHECK_CLOSE(5.0, polynomial(2.5), kTolerance);
}

TEST(EvaluatesLinearPolynomial)
{
    const Polynomial polynomial{2.0, 3.0};

    CHECK_CLOSE(2.0, polynomial(0.0), kTolerance);
    CHECK_CLOSE(14.0, polynomial(4.0), kTolerance);
    CHECK_CLOSE(-4.0, polynomial(-2.0), kTolerance);
}

TEST(EvaluatesQuadraticPolynomial)
{
    const Polynomial polynomial{1.0, -2.0, 3.0};

    CHECK_CLOSE(1.0, polynomial(0.0), kTolerance);
    CHECK_CLOSE(2.0, polynomial(1.0), kTolerance);
    CHECK_CLOSE(9.0, polynomial(2.0), kTolerance);
    CHECK_CLOSE(17.0, polynomial(-2.0), kTolerance);
}

TEST(CopyConstructorCreatesIndependentEqualPolynomial)
{
    Polynomial original{1.0, -2.0, 3.0};
    Polynomial copy(original);

    CHECK(original == copy);
    CHECK_EQUAL(original.degree(), copy.degree());

    copy[0] = 99.0;
    CHECK_CLOSE(1.0, original[0], kTolerance);
}

TEST(CopyAssignmentCopiesCoefficients)
{
    Polynomial source{3.0, -1.0, 0.5};
    Polynomial target{7.0};

    target = source;

    CHECK(source == target);
    CHECK_EQUAL(2, target.degree());
    CHECK_CLOSE(3.0, target[0], kTolerance);
    CHECK_CLOSE(-1.0, target[1], kTolerance);
    CHECK_CLOSE(0.5, target[2], kTolerance);

    target[1] = 8.0;
    CHECK_CLOSE(-1.0, source[1], kTolerance);
}

TEST(CopyAssignmentHandlesSelfAssignment)
{
    Polynomial polynomial{2.0, -3.0, 4.0};
    Polynomial& result = (polynomial = polynomial);

    CHECK(&result == &polynomial);
    CHECK_EQUAL(2, polynomial.degree());
    CHECK_CLOSE(2.0, polynomial[0], kTolerance);
    CHECK_CLOSE(-3.0, polynomial[1], kTolerance);
    CHECK_CLOSE(4.0, polynomial[2], kTolerance);
}

TEST(EqualityComparesDegreeAndCoefficients)
{
    const Polynomial polynomial{1.0, 2.0, 3.0};
    const Polynomial same{1.0, 2.0, 3.0};
    const Polynomial differentCoefficient{1.0, 2.0, 3.5};
    const Polynomial differentDegree{1.0, 2.0};

    CHECK(polynomial == same);
    CHECK(!(polynomial != same));
    CHECK(polynomial != differentCoefficient);
    CHECK(!(polynomial == differentCoefficient));
    CHECK(polynomial != differentDegree);
    CHECK(!(polynomial == differentDegree));
}

TEST(LeadingZeroNormalizationAffectsEquality)
{
    const Polynomial padded{1.0, 2.0, 0.0, 0.0};
    const Polynomial compact{1.0, 2.0};

    CHECK_EQUAL(1, padded.degree());
    CHECK(padded == compact);
    CHECK(!(padded != compact));
}

TEST(ZeroPolynomialHasDegreeZeroAndSingleZeroCoefficient)
{
    const Polynomial defaultZero;
    const Polynomial explicitZero{0.0};
    const Polynomial paddedZero{0.0, 0.0, 0.0};

    CHECK_EQUAL(0, defaultZero.degree());
    CHECK_CLOSE(0.0, defaultZero[0], kTolerance);
    CHECK(defaultZero == explicitZero);
    CHECK(defaultZero == paddedZero);
    CHECK_EQUAL(0, paddedZero.degree());
    CHECK_CLOSE(0.0, paddedZero[0], kTolerance);
    CHECK_THROW(defaultZero[1], std::out_of_range);
}

TEST(StreamOutputUsesDocumentedFormat)
{
    std::ostringstream output;
    output << Polynomial{1.0, -2.0, 3.0};

    CHECK_EQUAL(std::string("2 1 -2 3"), output.str());
}

TEST(StreamOutputFormatsZeroPolynomial)
{
    const Polynomial zero;
    std::ostringstream output;
    output << zero;

    CHECK_EQUAL(std::string("0 0"), output.str());
}

TEST(StreamInputRoundTripsStreamOutput)
{
    const Polynomial original{1.5, -2.0, 3.25};
    std::stringstream stream;
    stream << original;

    Polynomial parsed;
    stream >> parsed;

    CHECK(!stream.fail());
    CHECK(original == parsed);
    CHECK_EQUAL(original.degree(), parsed.degree());
}

TEST(StreamInputRejectsNegativeDegreeWithoutChangingTarget)
{
    Polynomial target{7.0, 8.0};
    const Polynomial original(target);
    std::istringstream input("-1 5");

    input >> target;

    CHECK(input.fail());
    CHECK(target == original);
}

TEST(StreamInputRejectsMissingCoefficientsWithoutChangingTarget)
{
    Polynomial target{7.0, 8.0};
    const Polynomial original(target);
    std::istringstream input("2 1 2");

    input >> target;

    CHECK(input.fail());
    CHECK(target == original);
}

TEST(StreamInputRejectsMalformedCoefficientWithoutChangingTarget)
{
    Polynomial target{7.0, 8.0};
    const Polynomial original(target);
    std::istringstream input("1 2 not-a-number");

    input >> target;

    CHECK(input.fail());
    CHECK(target == original);
}

TEST(StreamInputRejectsNonNumericDegreeWithoutChangingTarget)
{
    Polynomial target{7.0, 8.0};
    const Polynomial original(target);
    std::istringstream input("x 1 2");

    input >> target;

    CHECK(input.fail());
    CHECK(target == original);
}
