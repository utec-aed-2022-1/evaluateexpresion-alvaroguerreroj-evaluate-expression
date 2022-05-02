#include <cctype>
#include <functional>
#include <istream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>

#include "solution.hpp"

auto get_operator(char c) -> Operator
{
    switch (c)
    {
    case '*':
        return {1, [](double a, double b) { return a * b; }};
    case '/':
        return {1, [](double a, double b) { return a / b; }};
    case '+':
        return {2, [](double a, double b) { return a + b; }};
    case '-':
        return {2, [](double a, double b) { return a - b; }};
    case '(':
        return {99, [](double, double) { return double{}; }};
    default:
        throw std::runtime_error("Character not found");
    };
}

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

auto tokenize(std::string const& input) -> eval_container<symbol>
{
    std::istringstream in{input};

    eval_container<symbol> ret;

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
        try
        {
            std::visit(
                overload{
                    [&result](double d) { result.push_back(d); },
                    [&result](char c) {
                        if (result.size() < 2)
                        {
                            throw EvalError();
                        }
                        else
                        {
                            double a1 = result.back();
                            result.pop_back();
                            double a2 = result.back();
                            result.pop_back();

                            result.push_back(get_operator(c).fn(a2, a1));
                        }
                    }},
                e);
        }
        catch (EvalError&)
        {
            return {0, true};
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

auto operator<<(std::ostream& out, eval_container<std::variant<double, char>> const& v)
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
