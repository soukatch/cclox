#pragma once

#include <iostream>

enum struct token_type {
  l_paren__,
  r_paren__,
  l_brace__,
  r_brace__,
  dot__,
  semi__,
  comma__,
  plus__,
  minus__,
  star__,
  slash__,
  bang__,
  equal__,
  greater__,
  less__,
  bangequal__,
  equalequal__,
  greaterequal__,
  lessequal__,
  var__,
  if__,
  else__,
  for__,
  while__,
  return__,
  fun__,
  class__,
  true__,
  false__,
  nil__,
  string__,
  number__,
  identifier__,
  print__,
  eof__
};

std::ostream &operator<<(std::ostream &os, const token_type &type) {
  switch (type) {
    using enum token_type;
  case l_paren__:
    return os << "'('";
  case r_paren__:
    return os << "')'";
  case l_brace__:
    return os << "'{'";
  case r_brace__:
    return os << "'}'";
  case dot__:
    return os << "'.'";
  case semi__:
    return os << "';'";
  case comma__:
    return os << "','";
  case plus__:
    return os << "'+'";
  case minus__:
    return os << "'-'";
  case star__:
    return os << "'*'";
  case slash__:
    return os << "'/'";
  case bang__:
    return os << "'!'";
  case equal__:
    return os << "'='";
  case greater__:
    return os << "'>'";
  case less__:
    return os << "'<'";
  case bangequal__:
    return os << "'!='";
  case equalequal__:
    return os << "'=='";
  case greaterequal__:
    return os << "'>='";
  case lessequal__:
    return os << "'<='";
  case var__:
    return os << "'var'";
  case if__:
    return os << "'if'";
  case else__:
    return os << "'else'";
  case for__:
    return os << "'for'";
  case while__:
    return os << "'while'";
  case return__:
    return os << "'return'";
  case fun__:
    return os << "'fun'";
  case class__:
    return os << "'class'";
  case true__:
    return os << "'true'";
  case false__:
    return os << "'false'";
  case nil__:
    return os << "'nil'";
  case string__:
    return os << "string";
  case number__:
    return os << "number";
  case identifier__:
    return os << "identifier";
  case print__:
    return os << "'print'";
  case eof__:
    return os << "eof";
  }
}