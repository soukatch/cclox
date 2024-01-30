#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <variant>

enum struct expr_error { invalid_operands, undefined_identifier };

struct env final {
  std::unordered_map<std::string,
                     std::variant<double, std::string, bool, expr_error>>
      symbols_{};
  std::shared_ptr<env> prev_{};

  env(std::shared_ptr<env> prev) : prev_{prev} {}
  env() = default;
};

// std::vector<std::unordered_map<
//     std::string, std::variant<double, std::string, bool, expr_error>>>
//     env(1);