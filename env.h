#pragma once

#include <string>
#include <unordered_map>
#include <variant>

enum struct expr_error { invalid_operands, undefined_identifier };

std::vector<std::unordered_map<
    std::string, std::variant<double, std::string, bool, expr_error>>>
    env(1);