#include "BisectionMethod.h"
#include "NewtonsMethod.h"
#include <iostream>
#include <iomanip>
#include <cmath>

void printComparisonHeader() {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "         ROOT-FINDING METHODS COMPARISON\n";
    std::cout << std::string(80, '=') << "\n\n";
}

void compareMethod(const std::string& testName,
                   const std::string& functionDesc,
                   std::function<double(double)> f,
                   std::function<double(double)> fprime,
                   double intervalA, double intervalB,
                   double initialGuess,
                   double tolerance) {
    
    std::cout << "\n" << std::string(80, '-') << "\n";
    std::cout << "Test: " << testName << "\n";
    std::cout << "Function: " << functionDesc << "\n";
    std::cout << "Tolerance: " << tolerance << "\n";
    std::cout << std::string(80, '-') << "\n\n";
    
    // Bisection Method
    try {
        Config cfgBisect(tolerance, 100, intervalA, intervalB);
        BisectionMethod bisect(cfgBisect, f);
        double rootBisect = bisect.solve();
        
        std::cout << "BISECTION METHOD:\n";
        std::cout << "  Converged: " << (bisect.hasConverged() ? "Yes" : "No") << "\n";
        std::cout << "  Iterations: " << bisect.getIterationCount() << "\n";
        std::cout << "  Root: " << std::fixed << std::setprecision(12) << rootBisect << "\n";
        std::cout << "  f(root): " << std::scientific << std::setprecision(6) 
                  << f(rootBisect) << "\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "BISECTION METHOD FAILED: " << e.what() << "\n\n";
    }
    
    // Newton's Method (Analytical)
    try {
        Config cfgNewton(tolerance, 100, initialGuess);
        NewtonsMethod newton(cfgNewton, f, fprime);
        double rootNewton = newton.solve();
        
        std::cout << "NEWTON'S METHOD (Analytical):\n";
        std::cout << "  Converged: " << (newton.hasConverged() ? "Yes" : "No") << "\n";
        std::cout << "  Iterations: " << newton.getIterationCount() << "\n";
        std::cout << "  Root: " << std::fixed << std::setprecision(12) << rootNewton << "\n";
        std::cout << "  f(root): " << std::scientific << std::setprecision(6) 
                  << f(rootNewton) << "\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "NEWTON'S METHOD FAILED: " << e.what() << "\n\n";
    }
    
    // Newton's Method (Numerical)
    try {
        Config cfgNewtonNum(tolerance, 100, initialGuess);
        NewtonsMethod newtonNum(cfgNewtonNum, f);
        double rootNewtonNum = newtonNum.solve();
        
        std::cout << "NEWTON'S METHOD (Numerical):\n";
        std::cout << "  Converged: " << (newtonNum.hasConverged() ? "Yes" : "No") << "\n";
        std::cout << "  Iterations: " << newtonNum.getIterationCount() << "\n";
        std::cout << "  Root: " << std::fixed << std::setprecision(12) << rootNewtonNum << "\n";
        std::cout << "  f(root): " << std::scientific << std::setprecision(6) 
                  << f(rootNewtonNum) << "\n";
        
    } catch (const std::exception& e) {
        std::cout << "NEWTON'S METHOD (Numerical) FAILED: " << e.what() << "\n";
    }
}

int main() {
    printComparisonHeader();
    
    // Test 1: Square root of 2
    compareMethod(
        "Square Root of 2",
        "f(x) = x² - 2",
        [](double x) { return x*x - 2.0; },           // f(x)
        [](double x) { return 2.0*x; },               // f'(x)
        0.0, 3.0,                                     // interval [a, b]
        1.0,                                          // initial guess
        1e-10                                         // tolerance
    );
    
    // Test 2: Transcendental equation
    compareMethod(
        "Transcendental: cos(x) = x",
        "f(x) = cos(x) - x",
        [](double x) { return std::cos(x) - x; },
        [](double x) { return -std::sin(x) - 1.0; },
        0.0, 1.0,
        0.5,
        1e-10
    );
    
    // Test 3: Cubic polynomial
    compareMethod(
        "Cubic Polynomial",
        "f(x) = x³ - x - 2",
        [](double x) { return x*x*x - x - 2.0; },
        [](double x) { return 3.0*x*x - 1.0; },
        1.0, 2.0,
        1.5,
        1e-10
    );
    
    // Test 4: More challenging function
    compareMethod(
        "Exponential-Polynomial Mix",
        "f(x) = e^x - 3x",
        [](double x) { return std::exp(x) - 3.0*x; },
        [](double x) { return std::exp(x) - 3.0; },
        0.0, 2.0,
        1.0,
        1e-8
    );
    
    // Summary Statistics
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "KEY OBSERVATIONS:\n";
    std::cout << std::string(80, '=') << "\n";
    std::cout << "• Newton's Method: Quadratic convergence (iterations ≈ halves each time)\n";
    std::cout << "  - Typically 3-6 iterations for high precision\n";
    std::cout << "  - Requires derivative (analytical or numerical)\n";
    std::cout << "  - Can fail if derivative is zero or initial guess is poor\n\n";
    
    std::cout << "• Bisection Method: Linear convergence (one bit per iteration)\n";
    std::cout << "  - More iterations required (15-30 for high precision)\n";
    std::cout << "  - Guaranteed to converge if f(a)·f(b) < 0\n";
    std::cout << "  - Robust but slower\n\n";
    
    std::cout << "• Numerical vs Analytical Derivative:\n";
    std::cout << "  - Nearly identical performance in most cases\n";
    std::cout << "  - Numerical adds minimal overhead with good step size\n";
    std::cout << "  - Analytical is exact but requires explicit derivative function\n";
    std::cout << std::string(80, '=') << "\n";
    
    return 0;
}