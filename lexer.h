#pragma once

#include "token.h"
#include <unordered_map>
#include <vector>

namespace {
using enum token_type;
const std::unordered_map<std::string, token_type> keywords__{
    {"var", var__},     {"if", if__},       {"else", else__},
    {"for", for__},     {"while", while__}, {"return", return__},
    {"fun", fun__},     {"class", class__}, {"true", true__},
    {"false", false__}, {"nil", nil__},     {"print", print__}};
} // namespace

class lexer final {
public:
  lexer(std::string source) : source_{source} {}

  std::vector<token> scan() {
    for (; !is_end(); scan_token())
      ;

    add_token(token_type::eof__);

    return tokens_;
  }

private:
  void scan_token() {
    prev_ = next_;
    switch (const auto c{next()}; c) {
      using enum token_type;
    default:
      std::isalpha(c) ? identifier() : number_literal();
      break;
    case '(':
      add_token(l_paren__);
      break;
    case ')':
      add_token(r_paren__);
      break;
    case '{':
      add_token(l_brace__);
      break;
    case '}':
      add_token(r_brace__);
      break;
    case '.':
      add_token(dot__);
      break;
    case ',':
      add_token(comma__);
      break;
    case ';':
      add_token(semi__);
      break;
    case '+':
      add_token(plus__);
      break;
    case '-':
      add_token(minus__);
      break;
    case '*':
      add_token(star__);
      break;
    case '/':
      add_token(slash__);
      break;
    case '!':
      add_token(match('=') ? bangequal__ : bang__);
      break;
    case '=':
      add_token(match('=') ? equalequal__ : equal__);
      break;
    case '>':
      add_token(match('=') ? greaterequal__ : greater__);
      break;
    case '<':
      add_token(match('=') ? lessequal__ : less__);
      break;
    case '"':
      string_literal();
      break;
    case '\n':
      ++line_;
      [[fallthrough]];
    case ' ':
      [[fallthrough]];
    case '\r':
      [[fallthrough]];
    case '\t':;
    }
  }

  void identifier() {
    for (; std::isalpha(peek()); next())
      ;

    const auto text{source_.substr(prev_, next_ - prev_)};
    add_token(keywords__.contains(text) ? keywords__.at(text)
                                        : token_type::identifier__);
  }

  void string_literal() {
    for (; !is_end() && peek() != '"'; next())
      ;

    if (is_end())
      // handle error
      ;

    add_token(token_type::string__, prev_ + 1, next_);
    next();
  }

  void number_literal() {
    for (; std::isdigit(peek()); next())
      ;

    if (peek() == '.' && std::isdigit(peek_next()))
      for (next(); std::isdigit(peek()); next())
        ;

    add_token(token_type::number__, prev_, next_);
  }

  void add_token(token_type type) { add_token(type, prev_, next_); }

  void add_token(token_type type, std::string::size_type first,
                 std::string::size_type last) {
    tokens_.emplace_back(type, source_.substr(first, last - first), line_);
  }

  bool match(char c) noexcept { return c == peek() ? (++next_, true) : false; }

  char next() noexcept { return is_end() ? '\0' : source_[next_++]; }

  char peek() const noexcept { return is_end() ? '\0' : source_[next_]; }

  char peek_next() const noexcept {
    return next_ + 1 >= std::size(source_) ? '\0' : source_[next_ + 1];
  }

  bool is_end() const noexcept { return next_ >= std::size(source_); }

  const std::string source_{};
  std::vector<token> tokens_{};
  std::string::size_type prev_{}, next_{};
  int line_{};
};