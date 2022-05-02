#include <variant>
#include <vector>

#include "circular.hpp"
#include "solution.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("evaluate", "[evaluate]")
{
    using circ_list = CircularList<std::variant<double, char>>;

    CHECK(tokenize("(6 + 8) / (5 + 2) * 12")
          == circ_list{'(', 6.0, '+', 8.0, ')', '/', '(', 5.0, '+', 2.0, ')', '*', 12.0});
    CHECK(infix_to_postfix(tokenize("(6 + 8) / (5 + 2) * 12"))
          == circ_list{6.0, 8.0, '+', 5.0, 2.0, '+', '/', 12.0, '*'});

    CHECK(infix_to_postfix(tokenize("5 + 8 / 2")) == circ_list{5.0, 8.0, 2.0, '/', '+'});

    CHECK(evaluate("5 + 8 / 2") == Result{9, false});
    CHECK(evaluate("(7 + 8) / 2") == Result{7.5, false});
    CHECK(evaluate("(6 + 8) / (5 + 2)") == Result{2, false});
    CHECK(evaluate("(6 + 8) / (5 + 2) * 12") == Result{24, false});
    CHECK(evaluate("(6 + 8 / (5 + 2) * 3") == Result{0, true});
    CHECK(evaluate("(6 + 8) / (5 + 2) * 3 +") == Result{0, true});
    CHECK(evaluate("(6 + 8) 10 / (5 + 2) * 3 +") == Result{0, true});
}
