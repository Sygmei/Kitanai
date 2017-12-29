#pragma once

#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <random>

#include <Functions.hpp>
#include <Node.hpp>
#include <Token.hpp>

namespace ktn
{
    namespace StdLib
    {
        Node f_add(const std::vector<Node> tokens);
        Node f_sub(const std::vector<Node> tokens);
        Node f_mul(const std::vector<Node> tokens);
        Node f_div(const std::vector<Node> tokens);
        Node f_mod(const std::vector<Node> tokens);
        Node f_not(const std::vector<Node> tokens);
        Node f_eq(const std::vector<Node> tokens);
        Node f_neq(const std::vector<Node> tokens);
        Node f_gt(const std::vector<Node> tokens);
        Node f_ge(const std::vector<Node> tokens);
        Node f_lt(const std::vector<Node> tokens);
        Node f_le(const std::vector<Node> tokens);
        Node f_print(const std::vector<Node> tokens);
        Node f_input(const std::vector<Node> tokens);
        Node f_int(const std::vector<Node> tokens);
        Node f_string(const std::vector<Node> tokens);
        Node f_random(const std::vector<Node> tokens);
        Node f_time(const std::vector<Node> tokens);
        Node f_split(const std::vector<Node> tokens);
        Node f_read(const std::vector<Node> tokens);
        Node f_write(const std::vector<Node> tokens);
        Node fBuild(std::string funcname, int amount, std::function<Node(const std::vector<Node>)> func, std::vector<Node> parameters);
        std::pair<std::function<Node(const std::vector<Node>)>, int> fRightPart (std::string name, std::function<Node(const std::vector<Node>)> func, int amount);
        std::pair<std::string, std::pair<std::function<Node(const std::vector<Node>)>, int>> f(std::string name, std::function<Node(const std::vector<Node>)> func, int amount);
        static std::map<std::string, std::pair<std::function<Node(const std::vector<Node>)>, int>> FunctionsName = {
            f("add", f_add, 2),
            f("sub", f_sub, 2),
            f("mul", f_mul, 2),
            f("div", f_div, 2),
            f("mod", f_mod, 2),
            f("not", f_not, 1),
            f("eq", f_eq, 2),
            f("neq", f_neq, 2),
            f("gt", f_gt, 2),
            f("ge", f_ge, 2),
            f("lt", f_lt, 2),
            f("le", f_le, 2),
            f("print", f_print, 1),
            f("input", f_input, 1),
            f("string",  f_string, 1),
            f("int", f_int, 1),
            f("random", f_random, 0),
            f("time", f_time, 0),
            f("split", f_split, 3),
            f("read", f_read, 2),
            f("write", f_write, 2)
        };

        static std::map<std::string, Node*> customFunctions = {};
        Node fCall(std::string funcName, int expectedArgs, const std::vector<Node> tokens);
        Node f_func(const std::vector<Node> tokens);
    }
}