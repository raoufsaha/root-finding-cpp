#ifndef NEWTONSMETHOD_H
#define NEWTONSMETHOD_H

#include "RootFinder.h"
#include <optional>

/**
 * @brief Newton's Method for finding roots
 * 
 * Newton's method uses the iterative formula:
 *     x_{n+1} = x_n - f(x_n) / f'(x_n)
 * 
 * The method typically converges quadratically when started near a root,
 * making it much faster than bisection. However, it requires the derivative
 * and may fail if the derivative is zero or near zero.
 * 
 * Supports two modes:
 * 1. Analytical derivative: User provides f'(x) explicitly
 * 2. Numerical derivative: Automatically approximated using finite differences
 * 
 * Convergence criteria:
 * - |x_{n+1} - x_n| < tolerance (successive approximations)
 * - OR |f(x_n)| < tolerance (function value)
 * - OR max iterations reached
 */
class NewtonsMethod : public RootFinder {
private:
    double x0;  // Initial guess
    std::optional<std::function<double(double)>> derivative;  // Optional analytical derivative
    double h;   // Step size for numerical derivative (if used)
    bool useNumericalDerivative;
    
    /**
     * @brief Compute the derivative at a point
     * 
     * If analytical derivative is provided, uses it.
     * Otherwise, uses centered finite difference:
     *     f'(x) ≈ [f(x+h) - f(x-h)] / (2h)
     * 
     * @param x Point at which to evaluate derivative
     * @return Derivative value f'(x)
     */
    double computeDerivative(double x) const;
    
    /**
     * @brief Check if derivative is too close to zero
     * @param fprime Derivative value
     * @return true if derivative might cause numerical issues
     */
    bool isDerivativeTooSmall(double fprime) const {
        const double MIN_DERIVATIVE = 1e-12;
        return std::abs(fprime) < MIN_DERIVATIVE;
    }

public:
    /**
     * @brief Constructor with analytical derivative
     * @param cfg Configuration with initial guess, tolerance, and max iterations
     * @param f Function to find root of
     * @param fprime Analytical derivative f'(x)
     */
    NewtonsMethod(const Config& cfg, 
                  std::function<double(double)> f,
                  std::function<double(double)> fprime);
    
    /**
     * @brief Constructor with numerical derivative
     * @param cfg Configuration with initial guess, tolerance, and max iterations
     * @param f Function to find root of
     * @param stepSize Step size h for numerical derivative (default: 1e-6)
     */
    NewtonsMethod(const Config& cfg, 
                  std::function<double(double)> f,
                  double stepSize = 1e-6);
    
    /**
     * @brief Solve for root using Newton's method
     * 
     * Algorithm:
     * 1. Start with initial guess x_0
     * 2. Compute f(x_n) and f'(x_n)
     * 3. Check for zero derivative (would cause division by zero)
     * 4. Update: x_{n+1} = x_n - f(x_n) / f'(x_n)
     * 5. Check convergence based on |x_{n+1} - x_n| or |f(x_n)|
     * 6. Repeat until convergence or max iterations
     * 
     * @return Approximate root value
     * @throws std::runtime_error if derivative becomes zero during iteration
     */
    double solve() override;
    
    /**
     * @brief Get the method name
     * @return "Newton's Method (Analytical)" or "Newton's Method (Numerical)"
     */
    std::string getMethodName() const override {
        if (useNumericalDerivative) {
            return "Newton's Method (Numerical Derivative)";
        }
        return "Newton's Method (Analytical Derivative)";
    }
    
    /**
     * @brief Get initial guess used
     * @return Initial guess x_0
     */
    double getInitialGuess() const {
        return x0;
    }
    
    /**
     * @brief Check if using numerical derivative
     * @return true if numerical, false if analytical
     */
    bool isUsingNumericalDerivative() const {
        return useNumericalDerivative;
    }
};

#endif // NEWTONSMETHOD_H