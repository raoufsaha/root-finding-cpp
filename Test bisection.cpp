#include "BisectionMethod.h"
#include <iostream>
#include <cmath>

int main() {
    std::cout << "=== Bisection Method Test ===\n\n";
    
    // Test 1: f(x) = x^2 - 2 (root at sqrt(2) ≈ 1.414213562)
    std::cout << "Test 1: Finding sqrt(2) using f(x) = x^2 - 2\n";
    std::cout << std::string(50, '-') << "\n";
    
    auto f1 = [](double x) { return x * x - 2.0; };
    Config cfg1(1e-6, 50, 0.0, 3.0);  // tolerance, max_iter, interval [0, 3]
    
    try {
        BisectionMethod bisect1(cfg1, f1);
        double root1 = bisect1.solve();
        
        bisect1.printHistory();
        bisect1.printSummary();
        
        std::cout << "\nExpected root: " << std::sqrt(2.0) << "\n";
        std::cout << "Actual error: " << std::abs(root1 - std::sqrt(2.0)) << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // Test 2: f(x) = cos(x) - x (root ≈ 0.739085133)
    std::cout << "\n\nTest 2: Finding root of f(x) = cos(x) - x\n";
    std::cout << std::string(50, '-') << "\n";
    
    auto f2 = [](double x) { return std::cos(x) - x; };
    Config cfg2(1e-8, 100, 0.0, 1.0);  // Higher precision, interval [0, 1]
    
    try {
        BisectionMethod bisect2(cfg2, f2);
        double root2 = bisect2.solve();
        
        bisect2.printHistory();
        bisect2.printSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // Test 3: Error case - no sign change
    std::cout << "\n\nTest 3: Error handling - no sign change in interval\n";
    std::cout << std::string(50, '-') << "\n";
    
    auto f3 = [](double x) { return x * x + 1.0; };  // No real roots
    Config cfg3(1e-6, 50, 0.0, 2.0);
    
    try {
        BisectionMethod bisect3(cfg3, f3);
        bisect3.solve();
    } catch (const std::exception& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
    
    return 0;
}