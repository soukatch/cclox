#pragma once

#include "token.h"
#include <memory>
#include <variant>

template <typename T> inline constexpr auto isbool{std::is_same_v<T, bool>};
template <typename T> inline constexpr auto isnumber{std::is_same_v<T, double>};
template <typename T>
inline constexpr auto isstring{std::is_same_v<T, std::string>};

enum struct expr_error { invalid_operands };

struct expr {
  virtual std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept {}
};

struct binary_expr final : expr {
  const token op_{};
  const std::unique_ptr<expr> lhs_{}, rhs_{};

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    const auto x{lhs_->operator()()}, y{rhs_->operator()()};
    switch (op_.type_) {
      using enum token_type;
    case plus__:
      if (!std::is_same_v<decltype(x), decltype(y)> || isbool<decltype(x)>)
        return expr_error::invalid_operands;
      if (isnumber<decltype(x)>)
        return std::get<double>(x) + std::get<double>(y);
      else if (isstring<decltype(x)>)
        return std::get<std::string>(x) + std::get<std::string>(y);
    case minus__:
      if (isnumber<decltype(x)> && isnumber<decltype(y)>)
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case star__:
      if (isnumber<decltype(x)> && isnumber<decltype(y)>)
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case slash__:
      if (isnumber<decltype(x)> && isnumber<decltype(y)>)
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case equalequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) == std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) == std::get<std::string>(y)
                 : std::get<bool>(x) == std::get<bool>(y);
    case bangequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) != std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) != std::get<std::string>(y)
                 : std::get<bool>(x) != std::get<bool>(y);
    case greater__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) > std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) > std::get<std::string>(y)
                 : std::get<bool>(x) > std::get<bool>(y);
    case greaterequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) >= std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) >= std::get<std::string>(y)
                 : std::get<bool>(x) >= std::get<bool>(y);
    case less__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) < std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) < std::get<std::string>(y)
                 : std::get<bool>(x) < std::get<bool>(y);
    case lessequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return isnumber<decltype(x)> ? std::get<double>(x) <= std::get<double>(y)
             : isstring<decltype(y)>
                 ? std::get<std::string>(x) <= std::get<std::string>(y)
                 : std::get<bool>(x) <= std::get<bool>(y);
    case comma__:
      return y;
    }
  }
};

struct unary_expr final : expr {
  const token op_{};
  const std::unique_ptr<expr> rhs_{};

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    switch (const auto value{rhs_->operator()()}; op_.type_) {
      using enum token_type;
    case bang__:
      if (isnumber<decltype(value)>)
        return !std::get<double>(value);
      if (isbool<decltype(value)>)
        return !std::get<bool>(value);
      return expr_error::invalid_operands;
    case minus__:
      if (isnumber<decltype(value)>)
        return -std::get<double>(value);
      return expr_error::invalid_operands;
    }
  }
};

struct grouping_expr final : expr {
  const std::unique_ptr<expr> body_{};

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    return body_->operator()();
  }
};

struct literal_expr final : expr {
  const token literal_{};
  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    switch (literal_.type_) {
      using enum token_type;
    case number__:
      return std::stod(literal_.lexeme_);
    case string__:
      return literal_.lexeme_;
    case true__:
      return true;
    case false__:
      return false;
    }
  }
};