#pragma once

#include <chrono>
#include <variant>
#include <vector>

struct void_t final {};

struct function {
  std::vector<std::variant<double, std::string, bool>> args_{};
  std::variant<double, std::string, bool, void_t> operator()() const noexcept {}
};

struct clock final : function {
  std::variant<double, std::string, bool, void_t> operator()() const noexcept {
    return static_cast<double>(
               std::chrono::time_point_cast<std::chrono::milliseconds>(
                   std::chrono::high_resolution_clock::now())
                   .time_since_epoch()
                   .count()) /
           1000;
  }
};