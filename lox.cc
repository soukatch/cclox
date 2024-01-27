#include "lexer.h"
#include "parser.h"

#include <fstream>
#include <iostream>
#include <sysexits.h>

void run(std::string source) {
  lexer l{source};
  auto tokens{l.scan()};
  parser p{tokens};
  auto ast{p.parse()};

  auto x{ast->operator()()};

  if (is_bool(x))
    std::cout << std::boolalpha << std::get<bool>(x) << std::endl;
  else if (is_number(x))
    std::cout << std::get<double>(x) << std::endl;
  else if (is_string(x))
    std::cout << std::get<std::string>(x) << std::endl;
  else
    std::cout << "expr error encountered" << std::endl;
}

void run_prompt() {
  for (std::string line{}; (std::cout << "> ", std::getline(std::cin, line));)
    run(line);
}

void run_file(std::string path) {
  std::fstream f{path};
  run(std::string{std::istreambuf_iterator{f},
                  std::istreambuf_iterator<char>{}});
}

int main(int argc, char **argv) {
  switch (argc) {
  default:
    std::cout << "usage: lox [file]" << std::endl;
    exit(EX_USAGE);
  case 1:
    run_prompt();
    break;
  case 2:
    run_file(argv[1]);
  }
}