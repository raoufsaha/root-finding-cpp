#ifndef ROOTFINDER_H
#define ROOTFINDER_H

#include "Config.h"
#include <vector>
#include <functional>
#include <string>

/**
 * @brief Structure to store iteration data for logging
 */
struct IterationLog {
    int iteration;              // Iteration number
    double approximation;       // Current approximation x_n
    double error;               // |error| - interpretation varies by method
    double functionValue;       // f(x_n)
    
    IterationLog(int iter, double approx, double err, double fVal)
        : iteration(iter),
          approximation(approx),
          error(err),
          functionValue(fVal) {}
};

/**
 * @brief Abstract base class for root-finding methods
 * 
 * Provides a common interface for all root-finding algorithms.
 * Derived classes must implement the solve() method.
 */
class RootFinder {
protected:
    Config config;                          // Configuration parameters
    std::vector<IterationLog> history;      // Log of all iterations
    bool converged;                         // Convergence status
    int iterationCount;                     // Total iterations performed
    double root;                            // Final root value
    
    // Function to find root of (passed by user)
    std::function<double(double)> function;
    
    /**
     * @brief Logs an iteration to the history
     * @param iter Iteration number
     * @param approx Current approximation
     * @param err Error magnitude
     * @param fVal Function value at current approximation
     */
    void logIteration(int iter, double approx, double err, double fVal) {
        history.emplace_back(iter, approx, err, fVal);
    }
    
    /**
     * @brief Clears previous iteration history
     */
    void clearHistory() {
        history.clear();
        converged = false;
        iterationCount = 0;
        root = 0.0;
    }

public:
    /**
     * @brief Constructor
     * @param cfg Configuration parameters
     * @param f Function to find root of
     */
    RootFinder(const Config& cfg, std::function<double(double)> f)
        : config(cfg),
          converged(false),
          iterationCount(0),
          root(0.0),
          function(f) {
        config.validate();
    }
    
    /**
     * @brief Virtual destructor for proper cleanup of derived classes
     */
    virtual ~RootFinder() = default;
    
    /**
     * @brief Pure virtual function to solve for root
     * 
     * Must be implemented by derived classes.
     * Should return the approximate root and update convergence status.
     * 
     * @return Approximate root value
     */
    virtual double solve() = 0;
    
    /**
     * @brief Check if the method has converged
     * @return true if converged, false otherwise
     */
    bool hasConverged() const {
        return converged;
    }
    
    /**
     * @brief Get the number of iterations performed
     * @return Iteration count
     */
    int getIterationCount() const {
        return iterationCount;
    }
    
    /**
     * @brief Get the computed root value
     * @return Root value (only valid if converged)
     */
    double getRoot() const {
        return root;
    }
    
    /**
     * @brief Get the complete iteration history
     * @return Vector of iteration logs
     */
    const std::vector<IterationLog>& getIterationHistory() const {
        return history;
    }
    
    /**
     * @brief Get the method name (for display purposes)
     * @return Name of the method
     */
    virtual std::string getMethodName() const = 0;
    
    /**
     * @brief Print iteration history to console
     */
    void printHistory() const;
    
    /**
     * @brief Print a summary of the solution
     */
    void printSummary() const;
};

#endif // ROOTFINDER_H