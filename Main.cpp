#include "BisectionMethod.h"
#include "NewtonsMethod.h"
#include "ExprParser.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

// ============================================================
// Predefined test functions
// ============================================================
namespace TestFunctions {
    auto sqrt2             = [](double x) { return x*x - 2.0; };
    auto sqrt2_prime       = [](double x) { return 2.0*x; };

    auto cosine            = [](double x) { return std::cos(x) - x; };
    auto cosine_prime      = [](double x) { return -std::sin(x) - 1.0; };

    auto cubic             = [](double x) { return x*x*x - x - 2.0; };
    auto cubic_prime       = [](double x) { return 3.0*x*x - 1.0; };

    auto exponential       = [](double x) { return std::exp(x) - 3.0*x; };
    auto exponential_prime = [](double x) { return std::exp(x) - 3.0; };

    auto poly              = [](double x) { return x*x*x - 2.0*x + 2.0; };
    auto poly_prime        = [](double x) { return 3.0*x*x - 2.0; };
}

// ============================================================
// Input helpers
// ============================================================
void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string readLine(const std::string& prompt) {
    std::string line;
    std::cout << prompt;
    if (std::cin.peek() == '\n') std::cin.ignore();
    std::getline(std::cin, line);
    return line;
}

// ============================================================
// Derivative prompt — shared by preset and custom paths
// ============================================================

/**
 * @brief Ask the user whether to supply f'(x).
 *
 * For preset functions the built-in analytical derivative is offered as a
 * default so the user can simply press Enter to accept it.  For custom
 * expressions there is no built-in default, so the user must type one or
 * decline.
 *
 * @param fprime         In/out — already holds the built-in derivative when
 *                       hasBuiltIn is true; overwritten if the user types one.
 * @param hasAnalytical  Set to true when a valid derivative is available.
 * @param hasBuiltIn     True for preset functions (built-in derivative exists).
 * @param builtInLabel   Human-readable label shown to the user, e.g. "2*x".
 */
void askForDerivative(std::function<double(double)>& fprime,
                      bool& hasAnalytical,
                      bool hasBuiltIn,
                      const std::string& builtInLabel = "") {
    std::cout << "\n--- Derivative for Newton's Method ---\n";

    if (hasBuiltIn) {
        std::cout << "Built-in analytical derivative: f'(x) = " << builtInLabel << "\n";
        std::cout << "Options:\n";
        std::cout << "  1. Use built-in derivative (analytical)\n";
        std::cout << "  2. Enter my own derivative expression\n";
        std::cout << "  3. Use numerical derivative (auto)\n";
        std::cout << "Choice [1]: ";

        std::string line;
        if (std::cin.peek() == '\n') std::cin.ignore();
        std::getline(std::cin, line);

        // Default to 1 on blank Enter
        int sub = line.empty() ? 1 : std::stoi(line);

        if (sub == 1 || sub < 1 || sub > 3) {
            // Accept built-in (also default for out-of-range)
            hasAnalytical = true;
            std::cout << "Using built-in derivative.\n";
        } else if (sub == 2) {
            std::string dStr = readLine("Enter f'(x) = ");
            if (dStr.empty()) {
                std::cout << "Nothing entered — using built-in derivative.\n";
                hasAnalytical = true;
            } else {
                try {
                    ExprParser dParser(dStr);
                    fprime = dParser.toFunction();
                    hasAnalytical = true;
                    std::cout << "Custom derivative accepted.\n";
                } catch (const std::exception& e) {
                    std::cout << "Parse error: " << e.what()
                              << "\nFalling back to built-in derivative.\n";
                    hasAnalytical = true;   // still have built-in
                }
            }
        } else {
            // sub == 3
            hasAnalytical = false;
            std::cout << "Numerical derivative will be used.\n";
        }

    } else {
        // Custom function — no built-in derivative available
        std::cout << "Options:\n";
        std::cout << "  1. Enter f'(x) expression (analytical)\n";
        std::cout << "  2. Use numerical derivative (auto)\n";
        std::cout << "Choice: ";

        int sub;
        std::cin >> sub;

        if (sub == 1) {
            std::string dStr = readLine("Enter f'(x) = ");
            if (dStr.empty()) {
                std::cout << "Nothing entered — numerical derivative will be used.\n";
                hasAnalytical = false;
            } else {
                try {
                    ExprParser dParser(dStr);
                    fprime = dParser.toFunction();
                    hasAnalytical = true;
                    std::cout << "Derivative accepted.\n";
                } catch (const std::exception& e) {
                    std::cout << "Parse error: " << e.what()
                              << "\nFalling back to numerical derivative.\n";
                    hasAnalytical = false;
                }
            }
        } else {
            hasAnalytical = false;
            std::cout << "Numerical derivative will be used.\n";
        }
    }
}

// ============================================================
// Custom expression input
// ============================================================
bool getUserExpression(std::function<double(double)>& f,
                       std::function<double(double)>& fprime,
                       std::string& description,
                       bool& hasAnalytical) {
    std::cout << "\n--- Custom Expression Input ---\n";
    std::cout << "Operators : + - * / ^ ()\n";
    std::cout << "Functions : sin cos tan asin acos atan sinh cosh tanh\n";
    std::cout << "            sqrt abs exp log ln log2 log10 ceil floor\n";
    std::cout << "Constants : pi  e\n";
    std::cout << "Variable  : x\n";
    std::cout << "Examples  : x^3 - 2*x + 1\n";
    std::cout << "            sin(x) - x/2\n";
    std::cout << "            exp(x) - 3*x\n\n";

    std::string exprStr = readLine("Enter f(x) = ");
    if (exprStr.empty()) {
        std::cout << "No expression entered.\n";
        return false;
    }

    try {
        ExprParser parser(exprStr);
        f = parser.toFunction();
        description = "f(x) = " + exprStr;
    } catch (const std::exception& e) {
        std::cout << "Parse error: " << e.what() << "\n";
        return false;
    }

    // No built-in derivative for custom expressions
    askForDerivative(fprime, hasAnalytical, /*hasBuiltIn=*/false);
    return true;
}

// ============================================================
// Function selection menu
// ============================================================
void printFunctionMenu() {
    std::cout << "\nSelect a function:\n";
    std::cout << "  1. f(x) = x^2 - 2          (root ~1.414)\n";
    std::cout << "  2. f(x) = cos(x) - x        (root ~0.739)\n";
    std::cout << "  3. f(x) = x^3 - x - 2       (root ~1.521)\n";
    std::cout << "  4. f(x) = exp(x) - 3x       (root ~0.619)\n";
    std::cout << "  5. f(x) = x^3 - 2x + 2      (root ~-1.769)\n";
    std::cout << "  6. Enter my own expression\n";
    std::cout << "Choice: ";
}

bool getFunctionChoice(std::function<double(double)>& f,
                       std::function<double(double)>& fprime,
                       std::string& description,
                       bool& hasAnalytical) {
    int choice;
    std::cin >> choice;

    // Preset lookup table: { f, f', description, built-in label for prompt }
    struct Preset {
        std::function<double(double)> fn;
        std::function<double(double)> fnPrime;
        std::string desc;
        std::string primeLabel;
    };

    const Preset presets[] = {
        { TestFunctions::sqrt2,       TestFunctions::sqrt2_prime,
          "f(x) = x^2 - 2",          "2*x" },
        { TestFunctions::cosine,      TestFunctions::cosine_prime,
          "f(x) = cos(x) - x",       "-sin(x) - 1" },
        { TestFunctions::cubic,       TestFunctions::cubic_prime,
          "f(x) = x^3 - x - 2",      "3*x^2 - 1" },
        { TestFunctions::exponential, TestFunctions::exponential_prime,
          "f(x) = exp(x) - 3x",      "exp(x) - 3" },
        { TestFunctions::poly,        TestFunctions::poly_prime,
          "f(x) = x^3 - 2x + 2",     "3*x^2 - 2" },
    };

    if (choice >= 1 && choice <= 5) {
        const auto& p = presets[choice - 1];
        f           = p.fn;
        fprime      = p.fnPrime;   // loaded as default; may be overridden below
        description = p.desc;
        askForDerivative(fprime, hasAnalytical,
                         /*hasBuiltIn=*/true, p.primeLabel);
        return true;
    }

    if (choice == 6)
        return getUserExpression(f, fprime, description, hasAnalytical);

    std::cout << "Invalid choice.\n";
    return false;
}

// ============================================================
// Method runners
// ============================================================
void printMainMenu() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "        ROOT-FINDING METHODS PROGRAM\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "1. Bisection Method\n";
    std::cout << "2. Newton's Method (Analytical Derivative)\n";
    std::cout << "3. Newton's Method (Numerical Derivative)\n";
    std::cout << "4. Compare All Methods\n";
    std::cout << "5. Exit\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << "Select option: ";
}

void runBisection() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "BISECTION METHOD\n";
    std::cout << std::string(60, '=') << "\n";

    std::function<double(double)> f, fprime;
    std::string description;
    bool hasAnalytical;

    printFunctionMenu();
    if (!getFunctionChoice(f, fprime, description, hasAnalytical)) return;

    double a, b, tolerance;
    int maxIter;
    std::cout << "\nEnter interval start (a): "; std::cin >> a;
    std::cout << "Enter interval end   (b): "; std::cin >> b;
    std::cout << "Enter tolerance (e.g. 1e-6): "; std::cin >> tolerance;
    std::cout << "Enter max iterations: "; std::cin >> maxIter;

    try {
        Config cfg(tolerance, maxIter, a, b);
        BisectionMethod bisect(cfg, f);
        std::cout << "\nSolving " << description << "  on [" << a << ", " << b << "]\n";
        bisect.solve();
        bisect.printHistory();
        bisect.printSummary();
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void runNewtonAnalytical() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "NEWTON'S METHOD (Analytical Derivative)\n";
    std::cout << std::string(60, '=') << "\n";

    std::function<double(double)> f, fprime;
    std::string description;
    bool hasAnalytical;

    printFunctionMenu();
    if (!getFunctionChoice(f, fprime, description, hasAnalytical)) return;

    if (!hasAnalytical)
        std::cout << "\nNo analytical derivative — switching to numerical automatically.\n";

    double x0, tolerance;
    int maxIter;
    std::cout << "\nEnter initial guess (x0): "; std::cin >> x0;
    std::cout << "Enter tolerance (e.g. 1e-10): "; std::cin >> tolerance;
    std::cout << "Enter max iterations: "; std::cin >> maxIter;

    try {
        Config cfg(tolerance, maxIter, x0);
        std::cout << "\nSolving " << description << "  with x0 = " << x0 << "\n";
        if (hasAnalytical) {
            NewtonsMethod newton(cfg, f, fprime);
            newton.solve(); newton.printHistory(); newton.printSummary();
        } else {
            NewtonsMethod newton(cfg, f);
            newton.solve(); newton.printHistory(); newton.printSummary();
        }
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void runNewtonNumerical() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "NEWTON'S METHOD (Numerical Derivative)\n";
    std::cout << std::string(60, '=') << "\n";

    std::function<double(double)> f, fprime;
    std::string description;
    bool hasAnalytical;

    printFunctionMenu();
    if (!getFunctionChoice(f, fprime, description, hasAnalytical)) return;

    double x0, tolerance, stepSize;
    int maxIter;
    std::cout << "\nEnter initial guess (x0): "; std::cin >> x0;
    std::cout << "Enter tolerance (e.g. 1e-10): "; std::cin >> tolerance;
    std::cout << "Enter max iterations: "; std::cin >> maxIter;
    std::cout << "Enter step size h (e.g. 1e-6): "; std::cin >> stepSize;

    try {
        Config cfg(tolerance, maxIter, x0);
        NewtonsMethod newton(cfg, f, stepSize);
        std::cout << "\nSolving " << description << "  with x0 = " << x0 << "\n";
        newton.solve(); newton.printHistory(); newton.printSummary();
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
    }
}

void compareAllMethods() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "COMPARE ALL METHODS\n";
    std::cout << std::string(60, '=') << "\n";

    std::function<double(double)> f, fprime;
    std::string description;
    bool hasAnalytical;

    printFunctionMenu();
    if (!getFunctionChoice(f, fprime, description, hasAnalytical)) return;

    double a, b, x0, tolerance;
    int maxIter;
    std::cout << "\nFor Bisection — interval [a, b]:\n";
    std::cout << "  a: "; std::cin >> a;
    std::cout << "  b: "; std::cin >> b;
    std::cout << "\nFor Newton — initial guess x0: "; std::cin >> x0;
    std::cout << "\nShared settings:\n";
    std::cout << "  Tolerance:      "; std::cin >> tolerance;
    std::cout << "  Max iterations: "; std::cin >> maxIter;

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "RESULTS FOR: " << description << "\n";
    std::cout << std::string(60, '=') << "\n";

    try {
        Config cfgB(tolerance, maxIter, a, b);
        BisectionMethod bisect(cfgB, f);
        double rootB = bisect.solve();
        std::cout << "\n[1] BISECTION:\n";
        std::cout << "    Interval   : [" << a << ", " << b << "]\n";
        std::cout << "    Converged  : " << (bisect.hasConverged() ? "Yes" : "No") << "\n";
        std::cout << "    Iterations : " << bisect.getIterationCount() << "\n";
        std::cout << "    Root       : " << std::fixed << std::setprecision(12) << rootB << "\n";
    } catch (const std::exception& e) {
        std::cout << "\n[1] BISECTION: Failed — " << e.what() << "\n";
    }

    if (hasAnalytical) {
        try {
            Config cfgNA(tolerance, maxIter, x0);
            NewtonsMethod newtonA(cfgNA, f, fprime);
            double rootNA = newtonA.solve();
            std::cout << "\n[2] NEWTON (Analytical):\n";
            std::cout << "    x0         : " << x0 << "\n";
            std::cout << "    Converged  : " << (newtonA.hasConverged() ? "Yes" : "No") << "\n";
            std::cout << "    Iterations : " << newtonA.getIterationCount() << "\n";
            std::cout << "    Root       : " << std::fixed << std::setprecision(12) << rootNA << "\n";
        } catch (const std::exception& e) {
            std::cout << "\n[2] NEWTON (Analytical): Failed — " << e.what() << "\n";
        }
    } else {
        std::cout << "\n[2] NEWTON (Analytical): Skipped — no derivative provided\n";
    }

    try {
        Config cfgNN(tolerance, maxIter, x0);
        NewtonsMethod newtonN(cfgNN, f);
        double rootNN = newtonN.solve();
        std::cout << "\n[3] NEWTON (Numerical):\n";
        std::cout << "    x0         : " << x0 << "\n";
        std::cout << "    Converged  : " << (newtonN.hasConverged() ? "Yes" : "No") << "\n";
        std::cout << "    Iterations : " << newtonN.getIterationCount() << "\n";
        std::cout << "    Root       : " << std::fixed << std::setprecision(12) << rootNN << "\n";
    } catch (const std::exception& e) {
        std::cout << "\n[3] NEWTON (Numerical): Failed — " << e.what() << "\n";
    }

    std::cout << "\n" << std::string(60, '=') << "\n";
}

// ============================================================
// Entry point
// ============================================================
int main() {
    int choice;
    while (true) {
        printMainMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            clearInput();
            std::cout << "Invalid input. Please enter a number 1-5.\n";
            continue;
        }

        switch (choice) {
            case 1: runBisection();        break;
            case 2: runNewtonAnalytical(); break;
            case 3: runNewtonNumerical();  break;
            case 4: compareAllMethods();   break;
            case 5:
                std::cout << "\nGoodbye!\n";
                return 0;
            default:
                std::cout << "Please select 1-5.\n";
        }
    }
}