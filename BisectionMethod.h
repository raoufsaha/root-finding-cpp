#ifndef BISECTIONMETHOD_H
#define BISECTIONMETHOD_H

#include "RootFinder.h"

/**
 * @brief Bisection Method for finding roots
 * 
 * The bisection method finds a root by repeatedly halving an interval [a, b]
 * where f(a) and f(b) have opposite signs. The method is guaranteed to converge
 * for continuous functions but may be slower than Newton's method.
 * 
 * Convergence criteria:
 * - |b - a| < tolerance (interval width)
 * - OR |f(midpoint)| < tolerance (function value)
 * - OR max iterations reached
 */
class BisectionMethod : public RootFinder {
private:
    double a;  // Left endpoint of interval
    double b;  // Right endpoint of interval
    
    /**
     * @brief Check if function values have opposite signs
     * @param fa Function value at point a
     * @param fb Function value at point b
     * @return true if signs are opposite, false otherwise
     */
    bool oppositeSign(double fa, double fb) const {
        return (fa * fb) < 0.0;
    }

public:
    /**
     * @brief Constructor for Bisection Method
     * @param cfg Configuration with interval [a, b], tolerance, and max iterations
     * @param f Function to find root of
     * @throws std::invalid_argument if interval is invalid or f(a)*f(b) >= 0
     */
    BisectionMethod(const Config& cfg, std::function<double(double)> f);
    
    /**
     * @brief Solve for root using bisection method
     * 
     * Algorithm:
     * 1. Start with interval [a, b] where f(a)*f(b) < 0
     * 2. Compute midpoint c = (a + b) / 2
     * 3. If f(c) is close to 0 or interval is small enough, return c
     * 4. If f(a)*f(c) < 0, set b = c; otherwise set a = c
     * 5. Repeat until convergence or max iterations
     * 
     * @return Approximate root value
     */
    double solve() override;
    
    /**
     * @brief Get the method name
     * @return "Bisection Method"
     */
    std::string getMethodName() const override {
        return "Bisection Method";
    }
    
    /**
     * @brief Get current interval endpoints
     * @param left Output parameter for left endpoint
     * @param right Output parameter for right endpoint
     */
    void getInterval(double& left, double& right) const {
        left = a;
        right = b;
    }
};

#endif // BISECTIONMETHOD_H