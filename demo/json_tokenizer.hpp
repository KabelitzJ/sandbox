#ifndef DEMO_JSON_TOKENIZER_HPP_
#define DEMO_JSON_TOKENIZER_HPP_

#include <string>
#include <fstream>
#include <sstream>
#include <optional>

#include <types/primitives.hpp>

#include "json_node.hpp"

namespace demo {

class json_tokenizer {

public:

  enum class token_type : sbx::uint8 {
    object_begin,
    object_end,
    array_begin,
    array_end,
    comma,
    colon,
    string,
    number,
    boolean,
    null
  }; // enum class token

  struct token {
    token_type type{};
    std::optional<std::string> value{};
  }; // struct token

  json_tokenizer(const std::string& file_path)
  : _raw{},
    _cursor{0} {  
    _read_file(file_path);
  }

  ~json_tokenizer() = default;

  bool has_more_tokens() const noexcept {
    return _cursor < _raw.size();
  }

  token next_token() {
    _skip_whitespace_and_new_line();

    if (_cursor >= _raw.size()) {
      throw std::runtime_error{"Unexpected end of file"};
    }

    switch (_raw[_cursor]) {
      case '{': {
        _cursor++;
        return token{token_type::object_begin};
      }
      case '}': {
        _cursor++;
        return token{token_type::object_end};
      }
      case '[': {
        _cursor++;
        return token{token_type::array_begin};
      }
      case ']': {
        _cursor++;
        return token{token_type::array_end};
      }
      case ',': {
        _cursor++;
        return token{token_type::comma};
      }
      case ':': {
        _cursor++;
        return token{token_type::colon};
      }
      case 'n': {
        if (_raw.substr(_cursor, 4) == "null") {
          _cursor += 4;
          return token{token_type::null};
        }
        throw std::runtime_error{"Unexpected token: " + _raw.substr(_cursor, 4)};
      }
      case 't': {
        if (_raw.substr(_cursor, 4) == "true") {
          _cursor += 4;
          return token{token_type::boolean, "true"};
        }
        throw std::runtime_error{"Unexpected token: " + _raw.substr(_cursor, 4)};
      }
      case 'f': {
        if (_raw.substr(_cursor, 5) == "false") {
          _cursor += 5;
          return token{token_type::boolean, "false"};
        }
        throw std::runtime_error{"Unexpected token: " + _raw.substr(_cursor, 5)};
      }
      case '\"': {
        return token{token_type::string, _read_string()};
      }
      case '-':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9': {
        return token{token_type::number, _read_number()};
      }
      default: {
        throw std::runtime_error{"Unexpected token: " + _raw.substr(_cursor, 1)};
      }
    }
  }

private:

  void _read_file(const std::string& file_path) {
    auto file_stream = std::ifstream{file_path};

    if (!file_stream.is_open()) {
      throw std::runtime_error{"Failed to open file: " + file_path};
    }

    auto line = std::string{};
    auto buffer = std::stringstream{};

    while (std::getline(file_stream, line)) {
      buffer << line;
    }

    file_stream.close();

    _raw = buffer.str();
  }

  void _skip_whitespace_and_new_line() {
    while ((_raw[_cursor] == ' ' || _raw[_cursor] == '\n') && _cursor < _raw.size()) {
      _cursor++;
    }
  }

  std::string _read_string() {
    auto buffer = std::stringstream{};

    ++_cursor; // skip first '"'

    auto c = _raw[_cursor];

    while (c != '\"') {
      if (_cursor >= _raw.size()) {
        throw std::runtime_error{"Unexpected end of file"};
      }
      buffer << c;
      ++_cursor;
      c = _raw[_cursor];
    }

    ++_cursor; // skip last '"'

    return buffer.str();
  }

  std::string _read_number() {
    auto buffer = std::stringstream{};

    while (_raw[_cursor] != ' ' || _raw[_cursor] != '\n') {
      if (_cursor >= _raw.size()) {
        throw std::runtime_error{"Unexpected end of file"};
      }

      buffer << _raw[_cursor];
      _cursor++;
    }

    return buffer.str();
  }

  std::string _raw{};
  std::size_t _cursor{};

}; // class json_parser

} // namespace demo

#endif // DEMO_JSON_TOKENIZER_HPP_
