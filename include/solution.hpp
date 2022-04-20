#pragma once

#include <functional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "circular.hpp"

template<class T>
using eval_container = CircularList<T>;

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

auto get_operator(char c) -> Operator;

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
    enum class symbol_types
    {
        number,
        op,
        left_par,
        right_par
    };

    static auto symbol_type = [](symbol s) {
        if (std::holds_alternative<double>(s))
        {
            return symbol_types::number;
        }

        switch (std::get<char>(s))
        {
        case '(':
            return symbol_types::left_par;
        case ')':
            return symbol_types::right_par;
        default:
            return symbol_types::op;
        }
    };

    if (b == e)
    {
        throw InfixError();
    }

    container<symbol> postfix;
    container<char> ops;

    auto prev_s_type = symbol_types::left_par;
    for (auto it = b; it != e; it++)
    {
        auto s_type = symbol_type(*it);

        if (s_type == symbol_types::number)
        {
            if (prev_s_type == symbol_types::number || prev_s_type == symbol_types::right_par)
            {
                throw InfixError();
            }

            postfix.emplace_back(*it);
        }
        else
        {
            char c = std::get<char>(*it);

            if (s_type == symbol_types::left_par)
            {
                if (prev_s_type == symbol_types::number || prev_s_type == symbol_types::right_par)
                {
                    throw InfixError();
                }

                ops.push_back(c);
            }
            else if (s_type == symbol_types::right_par)
            {
                if (prev_s_type == symbol_types::op || prev_s_type == symbol_types::left_par)
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
                if (prev_s_type == symbol_types::op || prev_s_type == symbol_types::left_par)
                {
                    throw InfixError();
                }

                Operator const& cur_op = get_operator(c);
                char prev_op = 0;

                while (!ops.empty()
                       && (prev_op = ops.back(),
                           get_operator(prev_op).precedence <= cur_op.precedence))
                {
                    postfix.emplace_back(ops.back());
                    ops.pop_back();
                }

                ops.push_back(c);
            }
        }

        prev_s_type = s_type;
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
