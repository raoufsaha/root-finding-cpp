#include "NewtonsMethod.h"
#include <iostream>
#include <cmath>

int main() {
    std::cout << "=== Newton's Method Test ===\n\n";
    
    // ========================================================================
    // Test 1: f(x) = x^2 - 2 with ANALYTICAL derivative
    // ========================================================================
    std::cout << "Test 1a: Finding sqrt(2) with ANALYTICAL derivative\n";
    std::cout << "Function: f(x) = x^2 - 2, f'(x) = 2x\n";
    std::cout << std::string(70, '=') << "\n";
    
    auto f1 = [](double x) { return x * x - 2.0; };
    auto f1_prime = [](double x) { return 2.0 * x; };
    
    Config cfg1a(1e-10, 50, 1.0);  // tolerance, max_iter, initial guess
    
    try {
        NewtonsMethod newton1a(cfg1a, f1, f1_prime);
        double root1a = newton1a.solve();
        
        newton1a.printHistory();
        newton1a.printSummary();
        
        std::cout << "\nExpected root: " << std::sqrt(2.0) << "\n";
        std::cout << "Actual error: " << std::abs(root1a - std::sqrt(2.0)) << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // ========================================================================
    // Test 1b: Same function with NUMERICAL derivative
    // ========================================================================
    std::cout << "\n\nTest 1b: Finding sqrt(2) with NUMERICAL derivative\n";
    std::cout << "Function: f(x) = x^2 - 2\n";
    std::cout << std::string(70, '=') << "\n";
    
    Config cfg1b(1e-10, 50, 1.0);
    
    try {
        NewtonsMethod newton1b(cfg1b, f1);  // No derivative provided
        double root1b = newton1b.solve();
        
        newton1b.printHistory();
        newton1b.printSummary();
        
        std::cout << "\nExpected root: " << std::sqrt(2.0) << "\n";
        std::cout << "Actual error: " << std::abs(root1b - std::sqrt(2.0)) << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // ========================================================================
    // Test 2: f(x) = cos(x) - x with both derivative modes
    // ========================================================================
    std::cout << "\n\nTest 2a: f(x) = cos(x) - x with ANALYTICAL derivative\n";
    std::cout << "f'(x) = -sin(x) - 1\n";
    std::cout << std::string(70, '=') << "\n";
    
    auto f2 = [](double x) { return std::cos(x) - x; };
    auto f2_prime = [](double x) { return -std::sin(x) - 1.0; };
    
    Config cfg2a(1e-12, 50, 0.5);
    
    try {
        NewtonsMethod newton2a(cfg2a, f2, f2_prime);
        double root2a = newton2a.solve();
        
        newton2a.printHistory();
        newton2a.printSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    std::cout << "\n\nTest 2b: f(x) = cos(x) - x with NUMERICAL derivative\n";
    std::cout << std::string(70, '=') << "\n";
    
    Config cfg2b(1e-12, 50, 0.5);
    
    try {
        NewtonsMethod newton2b(cfg2b, f2);
        double root2b = newton2b.solve();
        
        newton2b.printHistory();
        newton2b.printSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // ========================================================================
    // Test 3: f(x) = x^3 - 2x + 2 (more complex polynomial)
    // ========================================================================
    std::cout << "\n\nTest 3: f(x) = x^3 - 2x + 2\n";
    std::cout << "Initial guess: x0 = -2.0\n";
    std::cout << std::string(70, '=') << "\n";
    
    auto f3 = [](double x) { return x*x*x - 2.0*x + 2.0; };
    auto f3_prime = [](double x) { return 3.0*x*x - 2.0; };
    
    Config cfg3(1e-10, 50, -2.0);
    
    try {
        NewtonsMethod newton3(cfg3, f3, f3_prime);
        double root3 = newton3.solve();
        
        newton3.printHistory();
        newton3.printSummary();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    
    // ========================================================================
    // Test 4: Error case - derivative becomes zero
    // ========================================================================
    std::cout << "\n\nTest 4: Error handling - zero derivative case\n";
    std::cout << "f(x) = x^2, starting at x0 = 0 (where f'(0) = 0)\n";
    std::cout << std::string(70, '=') << "\n";
    
    auto f4 = [](double x) { return x * x; };
    auto f4_prime = [](double x) { return 2.0 * x; };
    
    Config cfg4(1e-6, 50, 0.0);  // Start at x=0 where derivative is zero
    
    try {
        NewtonsMethod newton4(cfg4, f4, f4_prime);
        newton4.solve();
    } catch (const std::exception& e) {
        std::cout << "Caught expected error: " << e.what() << "\n";
    }
    
    // ========================================================================
    // Test 5: Performance comparison - convergence speed
    // ========================================================================
    std::cout << "\n\n" << std::string(70, '=') << "\n";
    std::cout << "CONVERGENCE COMPARISON SUMMARY\n";
    std::cout << std::string(70, '=') << "\n";
    
    auto f5 = [](double x) { return x*x*x - x - 2.0; };  // Root ≈ 1.5214
    auto f5_prime = [](double x) { return 3.0*x*x - 1.0; };
    
    Config cfg5a(1e-10, 50, 2.0);
    Config cfg5b(1e-10, 50, 2.0);
    
    NewtonsMethod analytical(cfg5a, f5, f5_prime);
    NewtonsMethod numerical(cfg5b, f5);
    
    analytical.solve();
    numerical.solve();
    
    std::cout << "Analytical derivative: " << analytical.getIterationCount() << " iterations\n";
    std::cout << "Numerical derivative:  " << numerical.getIterationCount() << " iterations\n";
    std::cout << std::string(70, '=') << "\n";
    
    return 0;
}