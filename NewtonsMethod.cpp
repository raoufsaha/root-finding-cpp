#include "NewtonsMethod.h"
#include <cmath>
#include <stdexcept>

/**
 * @brief Constructor with analytical derivative
 */
NewtonsMethod::NewtonsMethod(const Config& cfg,
                             std::function<double(double)> f,
                             std::function<double(double)> fprime)
    : RootFinder(cfg, f),
      x0(cfg.initialGuess),
      derivative(fprime),
      h(0.0),
      useNumericalDerivative(false) {
    // Config validation already done in RootFinder constructor
}

/**
 * @brief Constructor with numerical derivative
 */
NewtonsMethod::NewtonsMethod(const Config& cfg,
                             std::function<double(double)> f,
                             double stepSize)
    : RootFinder(cfg, f),
      x0(cfg.initialGuess),
      derivative(std::nullopt),
      h(stepSize),
      useNumericalDerivative(true) {
    
    if (stepSize <= 0.0) {
        throw std::invalid_argument("Step size for numerical derivative must be positive");
    }
}

/**
 * @brief Compute derivative using analytical or numerical approach
 */
double NewtonsMethod::computeDerivative(double x) const {
    if (useNumericalDerivative) {
        // Centered finite difference: f'(x) ≈ [f(x+h) - f(x-h)] / (2h)
        double fPlus = function(x + h);
        double fMinus = function(x - h);
        return (fPlus - fMinus) / (2.0 * h);
    } else {
        // Use analytical derivative
        return (*derivative)(x);
    }
}

/**
 * @brief Execute Newton's method to find a root
 */
double NewtonsMethod::solve() {
    // Clear any previous iteration history
    clearHistory();
    
    // Start with initial guess
    double xCurr = x0;
    double xNext = xCurr;
    
    // Main iteration loop
    for (int iter = 1; iter <= config.maxIterations; ++iter) {
        // Evaluate function and derivative at current point
        double fCurr = function(xCurr);
        double fPrime = computeDerivative(xCurr);
        
        // Check for zero or near-zero derivative
        if (isDerivativeTooSmall(fPrime)) {
            // Log the failed iteration
            logIteration(iter, xCurr, std::abs(xNext - xCurr), fCurr);
            iterationCount = iter;
            converged = false;
            root = xCurr;
            
            throw std::runtime_error(
                "Derivative is too close to zero at x = " + std::to_string(xCurr) +
                " (f'(x) = " + std::to_string(fPrime) + "). Cannot continue."
            );
        }
        
        // Newton's iteration: x_{n+1} = x_n - f(x_n) / f'(x_n)
        xNext = xCurr - (fCurr / fPrime);
        
        // Error is the change in x between iterations
        double error = std::abs(xNext - xCurr);
        
        // Log this iteration (before convergence check)
        logIteration(iter, xCurr, error, fCurr);
        
        iterationCount = iter;
        
        // Check convergence criteria
        // Criterion 1: Change in x is smaller than tolerance
        if (error < config.tolerance) {
            converged = true;
            root = xNext;
            return root;
        }
        
        // Criterion 2: Function value is close to zero
        if (std::abs(fCurr) < config.tolerance) {
            converged = true;
            root = xNext;
            return root;
        }
        
        // Update for next iteration
        xCurr = xNext;
    }
    
    // Max iterations reached without convergence
    converged = false;
    root = xNext;
    return root;
}