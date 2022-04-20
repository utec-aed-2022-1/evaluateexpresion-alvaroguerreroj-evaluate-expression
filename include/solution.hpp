#pragma once

#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

using symbol = std::variant<double, char>;

struct Result
{
    double result;
    bool error;

    auto operator==(Result const& other) const -> bool
    {
        return result == other.result && error == other.error;
    }

    friend auto operator<<(std::ostream& out, Result const& result) -> std::ostream&
    {
        out << "<" << result.result << ", " << result.error << ">";

        return out;
    }
};

class InfixError : public std::exception
{
public:
    [[nodiscard]] auto what() const noexcept -> char const* override
    {
        return "InfixError";
    }
};

auto evaluate(std::string const& input) -> Result;
auto infix_to_postfix(std::vector<symbol> const& vs) -> std::vector<symbol>;
auto tokenize(std::string const& input) -> std::vector<symbol>;

auto operator<<(std::ostream& out, std::variant<double, char> const& v) -> std::ostream&;
auto operator<<(std::ostream& out, std::vector<std::variant<double, char>> const& v)
    -> std::ostream&;
