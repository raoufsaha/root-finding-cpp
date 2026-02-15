# Root-Finding Methods in C++

A C++ implementation of two classical numerical root-finding algorithms — **Bisection** and **Newton's Method** — with a built-in expression parser so users can type any math function at runtime without recompiling.

---

## Features

- **Bisection Method** — guaranteed convergence via interval halving
- **Newton's Method** — fast quadratic convergence with two derivative modes:
  - Analytical — provide your own `f'(x)` expression
  - Numerical — automatic finite difference approximation
- **Runtime expression parser** — type functions like `sin(x) - x/2` or `x^3 - 2*x + 1` directly at the prompt
- **Per-iteration logging** — every iteration prints the current approximation, `|error|`, and `f(x)`
- **Configurable** — tolerance `ε`, max iterations, initial guess / interval all set at runtime
- **Interactive menu** — choose method, function, and derivative mode each run

---

## Getting Started

### Prerequisites

- C++17 compatible compiler (`g++`, `clang++`)
- `make`

### Build

```bash
# Build everything
make

# Or build just the main program
make root_finder
```

### Run

```bash
./root_finder
```

---

## Usage

On launch you'll see a menu:

```
============================================================
        ROOT-FINDING METHODS PROGRAM
============================================================
1. Bisection Method
2. Newton's Method (Analytical Derivative)
3. Newton's Method (Numerical Derivative)
4. Compare All Methods
5. Exit
```

### Preset functions

Five functions are built in with known analytical derivatives:

| # | Function | Root |
|---|----------|------|
| 1 | `x^2 - 2` | `√2 ≈ 1.4142` |
| 2 | `cos(x) - x` | `≈ 0.7391` |
| 3 | `x^3 - x - 2` | `≈ 1.5214` |
| 4 | `exp(x) - 3x` | `≈ 0.6191` |
| 5 | `x^3 - 2x + 2` | `≈ -1.7693` |

### Custom expressions (option 6)

Type any expression using:

| Category | Supported |
|----------|-----------|
| Operators | `+` `-` `*` `/` `^` `()` |
| Functions | `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` `sqrt` `abs` `exp` `log` `ln` `log2` `log10` `ceil` `floor` |
| Constants | `pi` `e` |
| Variable | `x` |

**Example session:**

```
Enter f(x) = x^3 - 2*x + cos(x)

--- Derivative for Newton's Method ---
Options:
  1. Enter f'(x) expression (analytical)
  2. Use numerical derivative (auto)
Choice: 1
Enter f'(x) = 3*x^2 - 2 - sin(x)
Derivative accepted.
```

### Derivative prompt

For Newton's method the program always asks how you want the derivative handled:

- **Preset functions** — offers the built-in derivative as default (just press Enter), lets you override with your own expression, or switch to numerical
- **Custom expressions** — prompts you to type `f'(x)` or use numerical automatically

---

## Output

Each run prints a full iteration table followed by a summary:

```
Newton's Method (Analytical) - Iteration History
======================================================================
 Iteration     Approximation           |Error|              f(x)
----------------------------------------------------------------------
         1      1.0000000000      5.000000e-01     -1.000000e+00
         2      1.5000000000      8.333333e-02      2.500000e-01
         3      1.4166666667      2.450980e-03      6.944444e-03
         4      1.4142156863      2.123900e-06      6.007305e-06
         5      1.4142135624      1.594724e-12      4.510614e-12
======================================================================

Newton's Method (Analytical) - Solution Summary
==================================================
Converged:        Yes
Iterations:       5
Root:             1.414213562373
f(root):          4.440892e-16
Tolerance:        1.000000e-10
==================================================
```

---

## Project Structure

```
.
├── Config.h               # Tolerance, max iterations, interval / initial guess
├── RootFinder.h/.cpp      # Abstract base class — iteration logging and output
├── BisectionMethod.h/.cpp # Bisection algorithm
├── NewtonsMethod.h/.cpp   # Newton's method (analytical + numerical derivative)
├── ExprParser.h           # Header-only recursive-descent expression parser
├── main.cpp               # Interactive menu-driven program
├── test_bisection.cpp     # Bisection test suite
├── test_newton.cpp        # Newton's method test suite
├── test_parser.cpp        # Expression parser unit tests (38 tests)
├── compare_methods.cpp    # Side-by-side method comparison
└── Makefile
```

---

## Running Tests

```bash
# Build and run all tests
make run_tests

# Or individually
make test_parser && ./test_parser
make test_bisection && ./test_bisection
make test_newton && ./test_newton
make compare_methods && ./compare_methods
```

The parser test suite covers arithmetic, operator precedence, all built-in functions, error handling, and integration with both solvers — **38/38 passing**.

---

## Performance

Both methods finding `√2` with tolerance `ε = 10⁻¹⁰`:

| Method | Iterations |
|--------|-----------|
| Bisection | 29 |
| Newton (Analytical) | 5 |
| Newton (Numerical) | 5 |

Newton's method converges quadratically — each iteration roughly doubles the number of correct decimal places. Bisection converges linearly, gaining about one bit of precision per iteration, but is guaranteed to converge as long as `f(a)` and `f(b)` have opposite signs.

---

## Built With

- C++17
- Standard library only — no external dependencies
