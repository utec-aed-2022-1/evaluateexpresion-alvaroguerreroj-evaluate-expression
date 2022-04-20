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

using function_double = std::function<double(double, double)>;

struct Operator
{
    unsigned precedence;
    function_double fn;
};

std::map<char, Operator> const operators = {
    {'*', {1, [](double a, double b) { return a * b; }}},
    {'/', {1, [](double a, double b) { return a / b; }}},
    {'+', {2, [](double a, double b) { return a + b; }}},
    {'-', {2, [](double a, double b) { return a - b; }}},
    {'(', {99, [](double, double) { return double{}; }}}
};

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

auto infix_to_postfix(std::vector<symbol> const& vs) -> std::vector<symbol>
{
    enum symbol_types
    {
        number,
        op,
        left_par,
        right_par
    };

    auto symbol_type = [](symbol s) {
        if (std::holds_alternative<double>(s))
        {
            return number;
        }

        switch (std::get<char>(s))
        {
        case '(':
            return left_par;
        case ')':
            return right_par;
        default:
            return op;
        }
    };

    std::vector<symbol> postfix;
    std::vector<char> ops;

    if (vs.empty())
    {
        throw InfixError();
    }

    auto it = vs.cbegin();
    if (std::holds_alternative<double>(*it))
    {
        postfix.push_back(*it);
    }
    else if (*it == symbol('('))
    {
        ops.push_back(std::get<char>(*it));
    }
    else
    {
        throw InfixError();
    }

    it++;

    for (; it != vs.cend(); it++)
    {
        auto s_type = symbol_type(*it);
        auto prev_s_type = symbol_type(*std::prev(it));

        if (s_type == number)
        {
            if (prev_s_type == number || prev_s_type == right_par)
            {
                throw InfixError();
            }

            postfix.emplace_back(*it);
        }
        else
        {
            char c = std::get<char>(*it);

            if (s_type == left_par)
            {
                if (prev_s_type == number || prev_s_type == right_par)
                {
                    throw InfixError();
                }

                ops.push_back(c);
            }
            else if (s_type == right_par)
            {
                if (prev_s_type == op || prev_s_type == left_par)
                {
                    throw InfixError();
                }

                char c = 0;
                while (c = ops.back(), ops.pop_back(), c != '(')
                {
                    postfix.emplace_back(c);
                }
            }
            else
            {
                if (prev_s_type == op || prev_s_type == left_par)
                {
                    throw InfixError();
                }

                Operator const& cur_op = operators.at(c);
                char prev_op = 0;

                while (!ops.empty()
                       && (prev_op = ops.back(),
                           operators.at(prev_op).precedence <= cur_op.precedence))
                {
                    postfix.emplace_back(ops.back());
                    ops.pop_back();
                }

                ops.push_back(c);
            }
        }
    }

    while (!ops.empty())
    {
        postfix.emplace_back(ops.back());
        ops.pop_back();
    }

    return postfix;
}

template<template<typename...> typename container = std::vector>
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
