#pragma once

#include <functional>
#include <map>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "circular.hpp"

template<class T>
using eval_container = std::vector<T>;

using symbol = std::variant<double, char>;
using function_double = std::function<double(double, double)>;

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

struct Operator
{
    unsigned precedence;
    function_double fn;

    Operator(unsigned _precedence, function_double _fn)
        : precedence(_precedence),
          fn(std::move(_fn))
    {
    }
};

std::map<char, Operator> const operators = {
    {'*', {1, [](double a, double b) { return a * b; }}},
    {'/', {1, [](double a, double b) { return a / b; }}},
    {'+', {2, [](double a, double b) { return a + b; }}},
    {'-', {2, [](double a, double b) { return a - b; }}},
    {'(', {99, [](double, double) { return double{}; }}}
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
auto tokenize(std::string const& input) -> eval_container<symbol>;

template<template<typename...> typename container, typename ForwardIterator>
auto infix_to_postfix(ForwardIterator b, ForwardIterator e) -> container<symbol>
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

    eval_container<symbol> postfix;
    eval_container<char> ops;

    if (b == e)
    {
        throw InfixError();
    }

    auto it = b;
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

    for (; it != e; it++)
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

template<template<typename...> typename result_container = eval_container,
    template<typename...> typename container>
auto infix_to_postfix(container<symbol> const& cn) -> result_container<symbol>
{
    return infix_to_postfix<result_container>(cn.begin(), cn.end());
}

auto operator<<(std::ostream& out, std::variant<double, char> const& v) -> std::ostream&;
auto operator<<(std::ostream& out, eval_container<std::variant<double, char>> const& v)
    -> std::ostream&;
