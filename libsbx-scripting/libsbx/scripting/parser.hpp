#ifndef LIBSBX_SCRIPTING_PARSER_HPP_
#define LIBSBX_SCRIPTING_PARSER_HPP_

#include <filesystem>
#include <vector>
#include <memory>

#include <libsbx/io/read_file.hpp>

#include <libsbx/scripting/lexer.hpp>

namespace sbx::scripting {

class parser {

  // The node of a abstract syntax tree.
  struct node {
    std::string name{};
    std::vector<std::unique_ptr<node>> children{};
  }; // struct node

public:

  parser(const std::filesystem::path& path) {
    auto file_content = io::read_file(path);
    auto lexer = scripting::lexer{file_content};
    _tokens = lexer.tokens();
  }

  auto parse() -> std::unique_ptr<node> {
    auto root = std::make_unique<node>();
    root->name = "root";

    while (_index < _tokens.size()) {
      auto token = _parse();
      root->children.push_back(std::move(token));
    }

    return root;
  }

private:

  auto _parse() -> std::unique_ptr<node> {
    auto token = _tokens[_index];
    auto child = std::make_unique<node>();

    if (token.type == token_type::identifier) {
      if (_index + 1 >= _tokens.size()) {
        throw std::runtime_error{"Unexpected end of file."};
      }

      ++_index;

      auto next = _tokens[_index];

      if (next.type == token_type::colon) {
        child = _parse_declaration();
      } else if (next.type == token_type::opening_parenthesis) {
        throw std::runtime_error{"Unexpected token."};
      }
    }

    return child;
  }

  auto _parse_declaration() -> std::unique_ptr<node> {
    auto child = std::make_unique<node>();

    auto token = _tokens[_index];
    
    if (token.type == token_type::identifier) {
      child->name = token.value;
    } else {
      throw std::runtime_error{"Unexpected token."};
    }

    ++_index;

    token = _tokens[_index];
    if (token.type == token_type::colon) {
      ++_index;
    } else {
      throw std::runtime_error{"Unexpected token."};
    }

    token = _tokens[_index];
    if (token.type == token_type::identifier) {
      child->children.push_back(std::make_unique<node>());
      child->children.back()->name = token.value;
    } else {
      throw std::runtime_error{"Unexpected token."};
    }

    ++_index;

    token = _tokens[_index];
    if (token.type == token_type::semicolon) {
      ++_index;
    } else {
      throw std::runtime_error{"Unexpected token."};
    }

    return child;
  }

  std::vector<token> _tokens{};
  std::size_t _index{0};

}; // class parser

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_PARSER_HPP_
