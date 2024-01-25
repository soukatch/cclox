#pragma once

#include "token_type.h"

#include <string>

struct token final {
  const token_type type_{};
  const std::string lexeme_{};
  const int line_{};
};