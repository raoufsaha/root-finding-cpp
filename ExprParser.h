#ifndef EXPRPARSER_H
#define EXPRPARSER_H

#include <string>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <functional>

// Define mathematical constants if not available
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E 2.71828182845904523536
#endif

/**
 * @brief Recursive-descent parser and evaluator for math expressions
 *
 * Supports:
 *   Operators : + - * / ^ (right-associative power)
 *   Unary     : - (negation)
 *   Functions : sin, cos, tan, asin, acos, atan,
 *               sqrt, abs, exp, log, log2, log10
 *   Constants : pi, e
 *   Variable  : x
 *   Grouping  : ( )
 *
 * Grammar (in order of increasing precedence):
 *   expr    = term   { ('+'|'-') term   }
 *   term    = factor { ('*'|'/') factor }
 *   factor  = base   { '^'        factor }   (right-assoc)
 *   base    = '-' base
 *           | number
 *           | identifier [ '(' expr ')' ]
 *           | '(' expr ')'
 *
 * Usage:
 *   ExprParser p("x^2 - 2");
 *   double val = p.evaluate(1.5);   // 0.25
 *
 *   // Or wrap as std::function for the solvers:
 *   auto f = p.toFunction();
 */
class ExprParser {
public:
    // Construct with an expression string
    explicit ExprParser(const std::string& expression)
        : expr_(expression), pos_(0), x_(0.0) {
        // Quick syntax check at construction time
        validate();
    }

    /**
     * @brief Evaluate the expression at x = val
     * @throws std::runtime_error on division by zero or domain error
     */
    double evaluate(double val) const {
        x_   = val;
        pos_ = 0;
        skipWhitespace();
        double result = parseExpr();
        skipWhitespace();
        if (pos_ != expr_.size()) {
            throw std::runtime_error(
                "Unexpected token at position " + std::to_string(pos_) +
                ": '" + expr_.substr(pos_) + "'");
        }
        return result;
    }

    /**
     * @brief Return a std::function<double(double)> wrapping this parser
     */
    std::function<double(double)> toFunction() const {
        // Capture by value so the returned function is self-contained
        ExprParser copy(*this);
        return [copy](double x) mutable { return copy.evaluate(x); };
    }

    /**
     * @brief Returns the original expression string
     */
    const std::string& expression() const { return expr_; }

    /**
     * @brief Quick check that the expression is syntactically valid
     * @throws std::runtime_error if the expression cannot be parsed
     */
    void validate() const {
        try {
            evaluate(0.0);   // parse with x=0; we only care about syntax
        } catch (const std::runtime_error& e) {
            throw std::runtime_error(
                std::string("Invalid expression: ") + e.what());
        }
    }

private:
    std::string      expr_;
    mutable size_t   pos_;
    mutable double   x_;

    // ---------------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------------
    void skipWhitespace() const {
        while (pos_ < expr_.size() && std::isspace((unsigned char)expr_[pos_]))
            ++pos_;
    }

    char peek() const {
        skipWhitespace();
        return pos_ < expr_.size() ? expr_[pos_] : '\0';
    }

    char consume() const {
        skipWhitespace();
        return expr_[pos_++];
    }

    bool match(char c) const {
        if (peek() == c) { ++pos_; return true; }
        return false;
    }

    void expect(char c) const {
        skipWhitespace();
        if (pos_ >= expr_.size() || expr_[pos_] != c)
            throw std::runtime_error(
                std::string("Expected '") + c + "' at position " +
                std::to_string(pos_));
        ++pos_;
    }

    // ---------------------------------------------------------------
    // Grammar rules
    // ---------------------------------------------------------------

    // expr = term { ('+' | '-') term }
    double parseExpr() const {
        double val = parseTerm();
        for (;;) {
            char op = peek();
            if (op == '+') { consume(); val += parseTerm(); }
            else if (op == '-') { consume(); val -= parseTerm(); }
            else break;
        }
        return val;
    }

    // term = factor { ('*' | '/') factor }
    double parseTerm() const {
        double val = parseFactor();
        for (;;) {
            char op = peek();
            if (op == '*') {
                consume(); val *= parseFactor();
            } else if (op == '/') {
                consume();
                double denom = parseFactor();
                if (denom == 0.0)
                    throw std::runtime_error("Division by zero");
                val /= denom;
            } else break;
        }
        return val;
    }

    // factor = base { '^' factor }   (right-associative)
    double parseFactor() const {
        double val = parseBase();
        if (peek() == '^') {
            consume();
            double exp = parseFactor();   // right-recursive
            val = std::pow(val, exp);
        }
        return val;
    }

    // base = '-' base | number | identifier ['(' expr ')'] | '(' expr ')'
    double parseBase() const {
        skipWhitespace();

        // Unary minus
        if (peek() == '-') {
            consume();
            return -parseBase();
        }

        // Parenthesised sub-expression
        if (peek() == '(') {
            consume();
            double val = parseExpr();
            expect(')');
            return val;
        }

        // Number literal
        if (std::isdigit((unsigned char)peek()) || peek() == '.') {
            return parseNumber();
        }

        // Identifier: variable or function name
        if (std::isalpha((unsigned char)peek()) || peek() == '_') {
            return parseIdentifier();
        }

        throw std::runtime_error(
            std::string("Unexpected character '") + peek() +
            "' at position " + std::to_string(pos_));
    }

    // Parse a floating-point number literal
    double parseNumber() const {
        size_t start = pos_;
        while (pos_ < expr_.size() &&
               (std::isdigit((unsigned char)expr_[pos_]) || expr_[pos_] == '.'))
            ++pos_;
        // Optional exponent  e.g. 1e-3
        if (pos_ < expr_.size() &&
            (expr_[pos_] == 'e' || expr_[pos_] == 'E')) {
            ++pos_;
            if (pos_ < expr_.size() &&
                (expr_[pos_] == '+' || expr_[pos_] == '-'))
                ++pos_;
            while (pos_ < expr_.size() &&
                   std::isdigit((unsigned char)expr_[pos_]))
                ++pos_;
        }
        return std::stod(expr_.substr(start, pos_ - start));
    }

    // Parse an identifier (variable 'x', constant, or function call)
    double parseIdentifier() const {
        size_t start = pos_;
        while (pos_ < expr_.size() &&
               (std::isalnum((unsigned char)expr_[pos_]) || expr_[pos_] == '_'))
            ++pos_;
        std::string name = expr_.substr(start, pos_ - start);

        // --- Constants ---
        if (name == "pi" || name == "PI") return M_PI;
        if (name == "e"  || name == "E")  return M_E;

        // --- Variable ---
        if (name == "x") return x_;

        // --- Functions (require parenthesised argument) ---
        skipWhitespace();
        expect('(');
        double arg = parseExpr();
        expect(')');

        if (name == "sin")   return std::sin(arg);
        if (name == "cos")   return std::cos(arg);
        if (name == "tan")   return std::tan(arg);
        if (name == "asin")  return std::asin(arg);
        if (name == "acos")  return std::acos(arg);
        if (name == "atan")  return std::atan(arg);
        if (name == "sinh")  return std::sinh(arg);
        if (name == "cosh")  return std::cosh(arg);
        if (name == "tanh")  return std::tanh(arg);
        if (name == "sqrt")  return std::sqrt(arg);
        if (name == "abs")   return std::fabs(arg);
        if (name == "exp")   return std::exp(arg);
        if (name == "log"  || name == "ln")   return std::log(arg);
        if (name == "log2")  return std::log2(arg);
        if (name == "log10") return std::log10(arg);
        if (name == "ceil")  return std::ceil(arg);
        if (name == "floor") return std::floor(arg);

        throw std::runtime_error("Unknown function or variable: '" + name + "'");
    }
};

#endif // EXPRPARSER_H