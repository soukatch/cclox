#pragma once

#include "stmt.h"
#include <format>
#include <initializer_list>
#include <sysexits.h>
#include <vector>

class parser final {
public:
  parser(std::vector<token> tokens) : tokens_{tokens} {}

  std::vector<std::unique_ptr<stmt>> make_ast() {
    parse();

    if (error_)
      stmts_.clear();

    return std::move(stmts_);
  }

  void parse() {
    error_stmt_ = false;
    for (; !is_end();)
      stmts_.push_back(statement());
  }

  std::unique_ptr<stmt> statement() {
    return match(token_type::print__) ? print_statement() : expr_statement();
  }

  std::unique_ptr<stmt> expr_statement() {
    auto s{std::make_unique<expr_stmt>(expression())};
    if (error_stmt_ || consume(token_type::semi__))
      return std::move(s);
    panic();
    return {};
  }

  std::unique_ptr<stmt> print_statement() {
    auto s{std::make_unique<print_stmt>(expression())};
    if (s->expr_ == nullptr)
      return {};
    if (error_stmt_ || consume(token_type::semi__))
      return std::move(s);
    panic();
    return {};
  }

  std::unique_ptr<expr> expression() {
    using enum token_type;
    auto lhs{equality()};

    for (; match(comma__);) {
      auto op{prev()};
      auto rhs{equality()};
      lhs = std::make_unique<binary_expr>(op, std::move(lhs), std::move(rhs));
    }

    return std::move(lhs);
  }

  std::unique_ptr<expr> equality() {
    using enum token_type;
    auto lhs{comparison()};

    for (; match({equalequal__, bangequal__});) {
      auto op{prev()};
      auto rhs{comparison()};
      lhs = std::make_unique<binary_expr>(op, std::move(lhs), std::move(rhs));
    }

    return std::move(lhs);
  }

  std::unique_ptr<expr> comparison() {
    using enum token_type;
    auto lhs{term()};

    for (; match({greater__, greaterequal__, less__, lessequal__});) {
      auto op{prev()};
      auto rhs{term()};
      lhs = std::make_unique<binary_expr>(op, std::move(lhs), std::move(rhs));
    }

    return std::move(lhs);
  }

  std::unique_ptr<expr> term() {
    using enum token_type;
    auto lhs{factor()};

    for (; match({plus__, minus__});) {
      auto op{prev()};
      auto rhs{factor()};
      lhs = std::make_unique<binary_expr>(op, std::move(lhs), std::move(rhs));
    }

    return std::move(lhs);
  }

  std::unique_ptr<expr> factor() {
    using enum token_type;
    auto lhs{unary()};

    for (; match({star__, slash__});) {
      auto op{prev()};
      auto rhs{unary()};
      lhs = std::make_unique<binary_expr>(op, std::move(lhs), std::move(rhs));
    }

    return lhs;
  }

  std::unique_ptr<expr> unary() {
    using enum token_type;
    return match({bang__, minus__})
               ? std::make_unique<unary_expr>(prev(), unary())
               : primary();
  }

  std::unique_ptr<expr> primary() {
    using enum token_type;

    if (match({number__, string__, identifier__, true__, false__, nil__}))
      return std::make_unique<literal_expr>(prev());

    if (match(l_paren__)) {
      auto e{expression()};
      if (consume(r_paren__))
        return std::move(e);
      panic();
      return {};
    }

    std::cerr << "expected expression." << std::endl;
    panic();
    return {};
  }

  void synchronize() {
    using enum token_type;
    for (; !is_end() && next().type_ != semi__;) {
      switch (peek().type_) {
      default:
        break;
      case if__:
        [[fallthrough]];
      case for__:
        [[fallthrough]];
      case return__:
        [[fallthrough]];
      case var__:
        [[fallthrough]];
      case class__:
        [[fallthrough]];
      case while__:
        [[fallthrough]];
      case print__:
        return;
      }
    }
  }

  void panic() {
    error_ = true;
    synchronize();
    parse();
    error_stmt_ = true;
  }

  bool consume(token_type type) {
    if (match(type))
      return true;
    std::cerr << "expected " << type << ", got " << peek().type_ << std::endl;
    return false;
  }

  bool match(std::initializer_list<token_type> types) {
    for (auto &&type : types)
      if (match(type))
        return true;
    return false;
  }

  bool match(token_type type) {
    return !is_end() && peek().type_ == type ? (++current_, true) : false;
  }

  token prev() const noexcept { return tokens_[current_ - 1]; }
  token next() { return tokens_[current_++]; }
  token peek() const noexcept { return tokens_[current_]; }
  bool is_end() const noexcept { return peek().type_ == token_type::eof__; }

  bool error_{}, error_stmt_{};
  std::vector<token> tokens_{};
  std::vector<token>::size_type current_{};
  std::vector<std::unique_ptr<stmt>> stmts_{};
};