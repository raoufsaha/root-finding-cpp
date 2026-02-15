#include "RootFinder.h"
#include <iostream>
#include <iomanip>
#include <cmath>

/**
 * @brief Print iteration history in a formatted table
 */
void RootFinder::printHistory() const {
    if (history.empty()) {
        std::cout << "No iteration history available.\n";
        return;
    }
    
    std::cout << "\n" << getMethodName() << " - Iteration History\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << std::setw(10) << "Iteration"
              << std::setw(18) << "Approximation"
              << std::setw(18) << "|Error|"
              << std::setw(18) << "f(x)"
              << "\n";
    std::cout << std::string(70, '-') << "\n";
    
    for (const auto& log : history) {
        std::cout << std::setw(10) << log.iteration
                  << std::setw(18) << std::fixed << std::setprecision(10) << log.approximation
                  << std::setw(18) << std::scientific << std::setprecision(6) << log.error
                  << std::setw(18) << std::scientific << std::setprecision(6) << log.functionValue
                  << "\n";
    }
    std::cout << std::string(70, '=') << "\n";
}

/**
 * @brief Print a summary of the solution process
 */
void RootFinder::printSummary() const {
    std::cout << "\n" << getMethodName() << " - Solution Summary\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Converged:        " << (converged ? "Yes" : "No") << "\n";
    std::cout << "Iterations:       " << iterationCount << "\n";
    
    if (converged) {
        std::cout << std::fixed << std::setprecision(12);
        std::cout << "Root:             " << root << "\n";
        std::cout << std::scientific << std::setprecision(6);
        std::cout << "f(root):          " << function(root) << "\n";
        std::cout << "Tolerance:        " << config.tolerance << "\n";
    } else {
        std::cout << "Status:           Failed to converge within " 
                  << config.maxIterations << " iterations\n";
        std::cout << "Last approximation: " << std::fixed << std::setprecision(10) 
                  << root << "\n";
    }
    std::cout << std::string(50, '=') << "\n";
}