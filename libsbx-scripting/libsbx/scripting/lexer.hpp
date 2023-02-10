#ifndef LIBSBX_SCRIPTING_LEXER_HPP_
#define LIBSBX_SCRIPTING_LEXER_HPP_

#include <string>
#include <vector>
#include <sstream>

namespace sbx::scripting {

enum class token_type {
  identifier,
  keyword,
  colon,
  comma,
  semicolon,
  equal,
  opening_parenthesis,
  closing_parenthesis,
  opening_bracket,
  closing_bracket,
  opening_brace,
  closing_brace,
  number,
  string,
  comment,
  return_type,
  op,
  eof
}; // enum class token_type

auto to_string(const token_type& type) -> std::string {
  switch (type) {
    case token_type::identifier: return "identifier";
    case token_type::keyword: return "keyword";
    case token_type::colon: return "colon";
    case token_type::comma: return "comma";
    case token_type::semicolon: return "semicolon";
    case token_type::equal: return "equal";
    case token_type::opening_parenthesis: return "opening_parenthesis";
    case token_type::closing_parenthesis: return "closing_parenthesis";
    case token_type::opening_bracket: return "opening_bracket";
    case token_type::closing_bracket: return "closing_bracket";
    case token_type::opening_brace: return "opening_brace";
    case token_type::closing_brace: return "closing_brace";
    case token_type::number: return "number";
    case token_type::string: return "string";
    case token_type::comment: return "comment";
    case token_type::return_type: return "return_type";
    case token_type::op: return "operator";
    case token_type::eof: return "eof";
  }

  return "unknown";
}

struct token {
  token_type type;
  std::string value;
}; // struct token

class lexer {

public:

  lexer(const std::string& source)
  : _source{source} {}

  auto tokens() -> std::vector<token> {
    auto tokens = std::vector<token>{};

    while (_index < _source.size()) {
      auto token = _next_token();
      tokens.push_back(token);
    }

    return tokens;
  }

private:

  auto _next_token() -> token {
    _skip_to_next_token();

    if (_index >= _source.size()) {
      return {token_type::eof, ""};
    }

    if (_current() == ':') {
      ++_index;
      return {token_type::colon, ":"};
    }

    if (_current() == ',') {
      ++_index;
      return {token_type::comma, ","};
    }

    if (_current() == ';') {
      ++_index;
      return {token_type::semicolon, ";"};
    }

    if (_current() == '=') {
      ++_index;
      return {token_type::equal, "="};
    }

    if (_current() == '(') {
      ++_index;
      return {token_type::opening_parenthesis, "("};
    }

    if (_current() == ')') {
      ++_index;
      return {token_type::closing_parenthesis, ")"};
    }

    if (_current() == '[') {
      ++_index;
      return {token_type::opening_bracket, "["};
    }

    if (_current() == ']') {
      ++_index;
      return {token_type::closing_bracket, "]"};
    }

    if (_current() == '{') {
      ++_index;
      return {token_type::opening_brace, "{"};
    }

    if (_current() == '}') {
      ++_index;
      return {token_type::closing_brace, "}"};
    }

    if (_current() == '\"') {
      return _string();
    }

    if (_current() == '-' && _peek() == '>') {
      _index += 2;
      return {token_type::return_type, "->"};
    }

    if (_current() == '/' && _peek() == '/') {
      return _comment();
    }

    if (_current() == '+' || _current() == '-' || _current() == '*' || _current() == '/' || _current() == '%' || _current() == '^' || _current() == '&' || _current() == '|' || _current() == '~' || _current() == '!' || _current() == '<' || _current() == '>') {
      const auto current = _current();
      ++_index;
      return {token_type::op, std::string{current}};
    }

    if (std::isdigit(_current())) {
      return _number();
    }

    return _identifier();
  }

  auto _skip_to_next_token() -> void {
    while (_index < _source.size()) {
      if (_current() == ' ' || _current() == '\n' || _current() == '\r' || _current() == '\t') {
        ++_index;
      } else {
        break;
      }
    }
  }

  auto _current() const -> char {
    if (_index < _source.size()) {
      return _source[_index];
    }

    return '\0';
  }

  auto _peek() const -> char {
    if (_index + 1 < _source.size()) {
      return _source[_index + 1];
    }

    return '\0';
  }

  auto _string() -> token {
    // Skip the opening quote.
    ++_index;

    auto value = std::stringstream{};

    while (_index < _source.size()) {
      if (_current() == '\"') {
        ++_index;
        return {token_type::string, value.str()};
      }

      value << _current();
      ++_index;
    }

    return {token_type::string, value.str()};
  }

  auto _comment() -> token {
    // Skip the opening slashes.
    _index += 2;


    auto value = std::stringstream{};

    while (_index < _source.size() && _current() != '\n') {
      value << _current();
      ++_index;
    }

    return {token_type::comment, value.str()};
  }

  auto _number() -> token {
    auto value = std::stringstream{};

    while (_index < _source.size() && std::isdigit(_current())) {
      value << _current();
      ++_index;
    }

    return {token_type::number, value.str()};
  }

  auto _identifier() -> token {
    auto value = std::stringstream{};

    while (_index < _source.size() && (std::isalnum(_current()) || _current() == '_')) {
      value << _current();
      ++_index;
    }

    return {token_type::identifier, value.str()};
  }

  std::string _source{};
  std::size_t _index{};

}; // class lexer

} // namespace sbx::scripting

#endif // LIBSBX_SCRIPTING_LEXER_HPP_
