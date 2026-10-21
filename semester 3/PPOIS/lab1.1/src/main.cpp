#include "Polynomial.h"

#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

void printCurrent(std::ostream& out, const Polynomial& a, const Polynomial& b)
{
    out << "\nCurrent polynomials (degree c0 c1 ... cdegree):\n"
        << "  A = " << a << '\n'
        << "  B = " << b << '\n';
}

void printMenu(std::ostream& out)
{
    out << "\nMenu:\n"
        << "  1. Read polynomial A\n"
        << "  2. Read polynomial B\n"
        << "  3. Display A and B\n"
        << "  4. Evaluate A at x\n"
        << "  5. Evaluate B at x\n"
        << "  6. Add A + B\n"
        << "  7. Subtract A - B\n"
        << "  8. Multiply A * B\n"
        << "  9. Divide A / B\n"
        << " 10. Compare A and B (==, !=)\n"
        << "  0. Exit\n";
}

template <typename T>
bool readNumber(std::istream& in, std::ostream& out,
                const std::string& prompt, T& value)
{
    while (true) {
        out << prompt;
        if (in >> value) {
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return true;
        }

        const bool endOfInput = in.eof();
        const bool brokenStream = in.bad();
        in.clear();
        if (endOfInput || brokenStream) {
            out << "\nEnd of input reached.\n";
            return false;
        }

        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out << "Invalid numeric input. Please try again.\n";
    }
}

bool readPolynomial(std::istream& in, std::ostream& out,
                    const std::string& name, Polynomial& polynomial)
{
    while (true) {
        out << "Enter polynomial " << name
            << " as: degree c0 c1 ... cdegree\n"
            << "Example: 2 1 2 3 means 1 + 2x + 3x^2\n"
            << name << "> ";

        Polynomial candidate;
        try {
            if (in >> candidate) {
                in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                polynomial = std::move(candidate);
                return true;
            }
        } catch (const std::exception& error) {
            out << "Could not read polynomial: " << error.what() << '\n';
        }

        const bool endOfInput = in.eof();
        const bool brokenStream = in.bad();
        in.clear();
        if (endOfInput || brokenStream) {
            out << "\nEnd of input reached.\n";
            return false;
        }

        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        out << "Invalid polynomial. The degree must be non-negative and "
               "followed by exactly degree + 1 coefficients.\n";
    }
}

}

int main()
{
    Polynomial a;
    Polynomial b;
    bool running = true;

    while (running) {
        printCurrent(std::cout, a, b);
        printMenu(std::cout);

        int choice = 0;
        if (!readNumber(std::cin, std::cout, "\nChoice> ", choice)) {
            break;
        }

        try {
            switch (choice) {
            case 1:
                if (!readPolynomial(std::cin, std::cout, "A", a)) {
                    running = false;
                }
                break;
            case 2:
                if (!readPolynomial(std::cin, std::cout, "B", b)) {
                    running = false;
                }
                break;
            case 3:
                printCurrent(std::cout, a, b);
                break;
            case 4: {
                double x = 0.0;
                if (!readNumber(std::cin, std::cout, "x> ", x)) {
                    running = false;
                    break;
                }
                std::cout << "A(" << x << ") = " << a(x) << '\n';
                break;
            }
            case 5: {
                double x = 0.0;
                if (!readNumber(std::cin, std::cout, "x> ", x)) {
                    running = false;
                    break;
                }
                std::cout << "B(" << x << ") = " << b(x) << '\n';
                break;
            }
            case 6:
                std::cout << "A + B = " << (a + b) << '\n';
                break;
            case 7:
                std::cout << "A - B = " << (a - b) << '\n';
                break;
            case 8:
                std::cout << "A * B = " << (a * b) << '\n';
                break;
            case 9:
                std::cout << "A / B = " << (a / b) << '\n';
                break;
            case 10:
                std::cout << "A == B: " << (a == b ? "true" : "false") << '\n'
                          << "A != B: " << (a != b ? "true" : "false") << '\n';
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Unknown menu option. Please choose 0-10.\n";
                break;
            }
        } catch (const std::invalid_argument& error) {
            std::cout << "Invalid argument: " << error.what() << '\n';
        } catch (const std::out_of_range& error) {
            std::cout << "Out of range: " << error.what() << '\n';
        } catch (const std::domain_error& error) {
            std::cout << "Domain error: " << error.what() << '\n';
        } catch (const std::exception& error) {
            std::cout << "Error: " << error.what() << '\n';
        } catch (...) {
            std::cout << "Unknown error.\n";
        }
    }

    std::cout << "\nGoodbye.\n";
    return 0;
}
