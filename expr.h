#pragma once

#include "env.h"
#include "token.h"
#include <memory>
#include <ranges>
#include <variant>

constexpr bool is_bool(auto &&x) noexcept {
  return std::holds_alternative<bool>(x);
}

constexpr bool is_number(auto &&x) noexcept {
  return std::holds_alternative<double>(x);
}

constexpr bool is_string(auto &&x) noexcept {
  return std::holds_alternative<std::string>(x);
}

constexpr bool is_error(auto &&x) noexcept {
  return std::holds_alternative<expr_error>(x);
}

std::ostream &operator<<(std::ostream &os, const expr_error &e) {
  switch (e) {
    using enum expr_error;
  case invalid_operands:
    return os << "invalid operands";
  case undefined_identifier:
    return os << "undefined identifier";
  }
}

bool to_bool(const std::variant<double, std::string, bool, expr_error> &value) {
  return is_error(value) || is_bool(value) && !std::get<bool>(value) ? false
                                                                     : true;
}

bool to_bool(std::variant<double, std::string, bool, expr_error> &&value) {
  return is_error(value) || is_bool(value) && !std::get<bool>(value) ? false
                                                                     : true;
}

struct expr {
  virtual std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept {
    return {};
  }
  virtual constexpr bool lvalue() const noexcept { return false; }
  virtual constexpr token identifier() const noexcept { return {}; }
};

struct assign_expr final : expr {
  token identifier_{};
  std::unique_ptr<expr> rhs_{};

  assign_expr(token identifier, std::unique_ptr<expr> rhs)
      : identifier_{identifier}, rhs_{std::move(rhs)} {}

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    for (auto &&e : std::views::reverse(env))
      if (e.contains(identifier_.lexeme_))
        return e[identifier_.lexeme_] = rhs_->operator()();

    std::cout << "undefined identifier " << identifier_.lexeme_ << std::endl;
    return expr_error::undefined_identifier;
  }
};

struct binary_expr final : expr {
  const token op_{};
  const std::unique_ptr<expr> lhs_{}, rhs_{};

  binary_expr(token op, std::unique_ptr<expr> lhs, std::unique_ptr<expr> rhs)
      : op_{op}, lhs_{std::move(lhs)}, rhs_{std::move(rhs)} {}

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    const auto x{lhs_->operator()()}, y{rhs_->operator()()};
    switch (op_.type_) {
      using enum token_type;
    default:
      return {};
    case plus__:
      if (!std::is_same_v<decltype(x), decltype(y)> || is_bool(x))
        return expr_error::invalid_operands;
      if (is_number(x))
        return std::get<double>(x) + std::get<double>(y);
      else if (is_string(x))
        return std::get<std::string>(x) + std::get<std::string>(y);
    case minus__:
      if (is_number(x) && is_number(y))
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case star__:
      if (is_number(x) && is_number(y))
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case slash__:
      if (is_number(x) && is_number(y))
        return std::get<double>(x) - std::get<double>(y);
      return expr_error::invalid_operands;
    case equalequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) == std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) == std::get<std::string>(y)
                 : std::get<bool>(x) == std::get<bool>(y);
    case bangequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) != std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) != std::get<std::string>(y)
                 : std::get<bool>(x) != std::get<bool>(y);
    case greater__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) > std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) > std::get<std::string>(y)
                 : std::get<bool>(x) > std::get<bool>(y);
    case greaterequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) >= std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) >= std::get<std::string>(y)
                 : std::get<bool>(x) >= std::get<bool>(y);
    case less__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) < std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) < std::get<std::string>(y)
                 : std::get<bool>(x) < std::get<bool>(y);
    case lessequal__:
      if (!std::is_same_v<decltype(x), decltype(y)>)
        return expr_error::invalid_operands;
      return is_number(x) ? std::get<double>(x) <= std::get<double>(y)
             : is_string(y)
                 ? std::get<std::string>(x) <= std::get<std::string>(y)
                 : std::get<bool>(x) <= std::get<bool>(y);
    case comma__:
      return y;
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

  literal_expr(token literal) : literal_{literal} {}

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    switch (literal_.type_) {
      using enum token_type;
    default:
      return {};
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

struct unary_expr final : expr {
  const token op_{};
  const std::unique_ptr<expr> rhs_{};

  unary_expr(token op, std::unique_ptr<expr> rhs)
      : op_{op}, rhs_{std::move(rhs)} {}

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    switch (const auto value{rhs_->operator()()}; op_.type_) {
      using enum token_type;
    default:
      return {};
    case bang__:
      if (is_number(value))
        return !std::get<double>(value);
      if (is_bool(value))
        return !std::get<bool>(value);
      return expr_error::invalid_operands;
    case minus__:
      if (is_number(value))
        return -std::get<double>(value);
      return expr_error::invalid_operands;
    }
  }
};

struct var_expr final : expr {
  token identifier_{};

  var_expr(token identifier) : identifier_{identifier} {}

  std::variant<double, std::string, bool, expr_error>
  operator()() const noexcept override {
    for (auto &&e : std::views::reverse(env))
      if (e.contains(identifier_.lexeme_))
        return e[identifier_.lexeme_];
    return expr_error::undefined_identifier;
  }

  constexpr bool lvalue() const noexcept override { return true; }

  constexpr token identifier() const noexcept override { return identifier_; }
};