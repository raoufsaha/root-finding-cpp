#include "BisectionMethod.h"
#include <cmath>
#include <stdexcept>

/**
 * @brief Constructor - validates interval and checks for sign change
 */
BisectionMethod::BisectionMethod(const Config& cfg, std::function<double(double)> f)
    : RootFinder(cfg, f),
      a(cfg.intervalStart),
      b(cfg.intervalEnd) {
    
    // Validate interval configuration
    config.validateInterval();
    
    // Check that function has opposite signs at endpoints
    double fa = function(a);
    double fb = function(b);
    
    if (!oppositeSign(fa, fb)) {
        throw std::invalid_argument(
            "Function must have opposite signs at interval endpoints. "
            "f(a) = " + std::to_string(fa) + ", f(b) = " + std::to_string(fb)
        );
    }
}

/**
 * @brief Execute the bisection method to find a root
 */
double BisectionMethod::solve() {
    // Clear any previous iteration history
    clearHistory();
    
    // Initialize working interval
    double currA = a;
    double currB = b;
    double midpoint = 0.0;
    
    // Evaluate function at endpoints
    double fa = function(currA);
    double fb = function(currB);
    
    // Main iteration loop
    for (int iter = 1; iter <= config.maxIterations; ++iter) {
        // Compute midpoint
        midpoint = (currA + currB) / 2.0;
        double fMid = function(midpoint);
        
        // Current interval width (our error measure for bisection)
        double intervalWidth = std::abs(currB - currA);
        
        // Log this iteration
        // For bisection: error = interval width, approximation = midpoint
        logIteration(iter, midpoint, intervalWidth, fMid);
        
        iterationCount = iter;
        
        // Check convergence criteria
        // Criterion 1: Interval width is smaller than tolerance
        if (intervalWidth < config.tolerance) {
            converged = true;
            root = midpoint;
            return root;
        }
        
        // Criterion 2: Function value is close to zero
        if (std::abs(fMid) < config.tolerance) {
            converged = true;
            root = midpoint;
            return root;
        }
        
        // Update interval for next iteration
        // If f(a) and f(mid) have opposite signs, root is in [a, mid]
        // Otherwise, root is in [mid, b]
        if (oppositeSign(fa, fMid)) {
            currB = midpoint;
            fb = fMid;
        } else {
            currA = midpoint;
            fa = fMid;
        }
    }
    
    // Max iterations reached without convergence
    converged = false;
    root = midpoint;  // Return best approximation
    return root;
}