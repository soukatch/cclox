#include "expr.h"

#include <initializer_list>
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
  virtual void operator()(std::shared_ptr<env>) const noexcept {}
};

struct block_stmt final : stmt {
  std::vector<std::unique_ptr<stmt>> stmts_{};

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    for (auto scope{std::make_shared<env>(environ)}; auto &&x : stmts_)
      x->operator()(scope);
  }
};

struct decl_stmt final : stmt {
  token identifier_{};
  std::unique_ptr<expr> value_{};

  decl_stmt(token identifier, std::unique_ptr<expr> value)
      : identifier_{identifier}, value_{std::move(value)} {}

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    if (environ->symbols_.contains(identifier_.lexeme_)) {
      std::cerr << identifier_.lexeme_ << " already declared." << std::endl;
      return;
    }

    environ->symbols_[identifier_.lexeme_] =
        value_ == nullptr
            ? std::variant<double, std::string, bool, expr_error>{}
            : value_->operator()(environ);
  }
};

struct expr_stmt final : stmt {
  std::unique_ptr<expr> expr_{};

  expr_stmt(std::unique_ptr<expr> e) : expr_{std::move(e)} {}

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    expr_->operator()(environ);
  }
};

struct fun_stmt final : stmt {
  token name_{};
  std::vector<token> params_{};
  std::unique_ptr<stmt> body_{};

  fun_stmt(token &&name, std::vector<token> &&params,
           std::unique_ptr<stmt> body)
      : name_{std::move(name)}, params_{std::move(params)},
        body_{std::move(body)} {}
};

struct if_stmt final : stmt {
  std::unique_ptr<expr> condition_{};
  std::unique_ptr<stmt> if_branch_{}, else_branch_{};

  if_stmt(std::unique_ptr<expr> condition, std::unique_ptr<stmt> if_branch,
          std::unique_ptr<stmt> else_branch = nullptr)
      : condition_{std::move(condition)}, if_branch_{std::move(if_branch)},
        else_branch_{std::move(else_branch)} {}

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    if (to_bool(condition_->operator()(environ)))
      if_branch_->operator()(environ);
    else if (else_branch_ != nullptr)
      else_branch_->operator()(environ);
  }
};

struct print_stmt final : stmt {
  std::unique_ptr<expr> expr_{};

  print_stmt(std::unique_ptr<expr> e) : expr_{std::move(e)} {}

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    std::cout << expr_->operator()(environ) << std::endl;
  }
};

struct while_stmt final : stmt {
  std::unique_ptr<expr> condition_{};
  std::unique_ptr<stmt> body_{};

  while_stmt(std::unique_ptr<expr> condition, std::unique_ptr<stmt> body)
      : condition_{std::move(condition)}, body_{std::move(body)} {}

  void operator()(std::shared_ptr<env> environ) const noexcept override {
    for (; to_bool(condition_->operator()(environ));)
      body_->operator()(environ);
  }
};