#pragma once

#include "token.h"
#include <memory>

struct expr {
  virtual void operator()() const noexcept {}
};

struct binary_expr final : expr {
  token_type op_{};
  const std::unique_ptr<expr> lhs_{}, rhs_{};
};

struct unary_expr final : expr {
  token_type op_{};
  const std::unique_ptr<expr> rhs_{};
};

struct literal_expr final : expr {
  token literal_{};
};