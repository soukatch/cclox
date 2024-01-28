#include "expr.h"

#include <iostream>

std::ostream &
operator<<(std::ostream &os,
           const std::variant<double, std::string, bool, expr_error> v) {
  if (is_number(v))
    return os << std::get<double>(v);
  if (is_string(v))
    return os << std::get<std::string>(v);
  if (is_bool(v))
    return os << std::get<bool>(v);
  return os << std::get<expr_error>(v);
}

struct stmt {
  virtual void operator()() const noexcept {}
};

struct decl_stmt final : stmt {
  token identifier_{};
  std::unique_ptr<expr> value_{};

  decl_stmt(token identifier, std::unique_ptr<expr> value)
      : identifier_{identifier}, value_{std::move(value)} {}

  void operator()() const noexcept override {
    if (env.contains(identifier_.lexeme_)) {
      std::cerr << identifier_.lexeme_ << " already declared." << std::endl;
      return;
    }

    env[identifier_.lexeme_] = value_->operator()();
  }
};

struct expr_stmt final : stmt {
  std::unique_ptr<expr> expr_{};

  expr_stmt(std::unique_ptr<expr> e) : expr_{std::move(e)} {}

  void operator()() const noexcept override { expr_->operator()(); }
};

struct print_stmt final : stmt {
  std::unique_ptr<expr> expr_{};

  print_stmt(std::unique_ptr<expr> e) : expr_{std::move(e)} {}

  void operator()() const noexcept override {
    std::cout << expr_->operator()() << std::endl;
  }
};