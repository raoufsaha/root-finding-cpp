#ifndef CONFIG_H
#define CONFIG_H

#include <stdexcept>

/**
 * @brief Configuration structure for root-finding methods
 * 
 * Contains all configurable parameters shared across different
 * root-finding algorithms (Bisection, Newton's, etc.)
 */
struct Config {
    // Convergence criteria
    double tolerance;           // Tolerance ε for convergence
    int maxIterations;          // Maximum number of iterations allowed
    
    // Method-specific parameters
    double initialGuess;        // For Newton's method (x₀)
    double intervalStart;       // For Bisection method (a)
    double intervalEnd;         // For Bisection method (b)
    
    /**
     * @brief Default constructor with reasonable default values
     */
    Config() 
        : tolerance(1e-6),
          maxIterations(100),
          initialGuess(0.0),
          intervalStart(0.0),
          intervalEnd(1.0) {}
    
    /**
     * @brief Constructor for Newton's method
     * @param tol Convergence tolerance
     * @param maxIter Maximum iterations
     * @param x0 Initial guess
     */
    Config(double tol, int maxIter, double x0)
        : tolerance(tol),
          maxIterations(maxIter),
          initialGuess(x0),
          intervalStart(0.0),
          intervalEnd(1.0) {
        validate();
    }
    
    /**
     * @brief Constructor for Bisection method
     * @param tol Convergence tolerance
     * @param maxIter Maximum iterations
     * @param a Interval start
     * @param b Interval end
     */
    Config(double tol, int maxIter, double a, double b)
        : tolerance(tol),
          maxIterations(maxIter),
          initialGuess(0.0),
          intervalStart(a),
          intervalEnd(b) {
        validate();
    }
    
    /**
     * @brief Validates configuration parameters
     * @throws std::invalid_argument if parameters are invalid
     */
    void validate() const {
        if (tolerance <= 0.0) {
            throw std::invalid_argument("Tolerance must be positive");
        }
        if (maxIterations <= 0) {
            throw std::invalid_argument("Max iterations must be positive");
        }
    }
    
    /**
     * @brief Validates interval for bisection method
     * @throws std::invalid_argument if interval is invalid
     */
    void validateInterval() const {
        validate();
        if (intervalStart >= intervalEnd) {
            throw std::invalid_argument("Interval start must be less than interval end");
        }
    }
};

#endif // CONFIG_H