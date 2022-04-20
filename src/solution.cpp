#include <cctype>
#include <functional>
#include <istream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "solution.hpp"

auto read_double(std::istringstream& in) -> double
{
    double ret = 0;
    in >> ret;

    return ret;
}

auto next_token(std::istringstream& in) -> symbol
{
    if (std::isdigit(in.peek()) != 0)
    {
        return read_double(in);
    }

    return char(in.get());
}

auto tokenize(std::string const& input) -> std::vector<symbol>
{
    std::istringstream in{input};

    std::vector<symbol> ret;

    while (in >> std::ws, !in.eof())
    {
        ret.push_back(next_token(in));
    }

    return ret;
}

template<template<typename...> typename container>
auto evaluate(container<symbol> const& cn) -> Result
{
    if (cn.empty())
    {
        return {0, true};
    }

    container<double> result;
    for (auto const& e : cn)
    {
        if (std::holds_alternative<double>(e))
        {
            result.push_back(std::get<double>(e));
        }
        else if (result.size() < 2)
        {
            return {0, true};
        }
        else
        {
            double a1 = result.back();
            result.pop_back();
            double a2 = result.back();
            result.pop_back();

            result.push_back(operators.at(std::get<char>(e)).fn(a2, a1));
        }
    }

    if (result.size() != 1)
    {
        return {0, true};
    }

    return {result.back(), false};
}

auto evaluate(std::string const& input) -> Result
{
    // 1- descomponer el input y validar

    // 2- convertir de Infijo a Postfijo

    // 3- resolver la expresion

    //* Si no cumple la validacion retornar Result.error = true;

    try
    {
        auto infix = infix_to_postfix(tokenize(input));

        return evaluate(infix);
    }
    catch (InfixError&)
    {
        return {0, true};
    }
}

auto operator<<(std::ostream& out, std::variant<double, char> const& v) -> std::ostream&
{
    std::visit([&out](auto&& e) { out << e; }, v);

    return out;
}

auto operator<<(std::ostream& out, std::vector<std::variant<double, char>> const& v)
    -> std::ostream&
{
    out << "{ ";
    for (auto const& it : v)
    {
        out << it << " ";
    }

    out << "}";

    return out;
}
