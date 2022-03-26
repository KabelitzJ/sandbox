#ifndef DEMO_JSON_PARSER_HPP_
#define DEMO_JSON_PARSER_HPP_

#include <memory>
#include <string>

#include "json_node.hpp"
#include "json_tokenizer.hpp"

namespace demo {

class json_parser {

public:

  json_parser(const std::string& file_path)
  : _tokenizer{file_path},
    _current{},
    _root{} { }

  ~json_parser() = default;

  std::shared_ptr<json_node> root() {
    return _root;
  }

  void parse() {
    while (_tokenizer.has_more_tokens()) {
      auto token = _tokenizer.next_token();

      switch (token.type) {
        case json_tokenizer::token_type::object_begin: {
          auto object = _parse_object();

          if (!_root) {
            _root = object;
          }

          break;
        }
        case json_tokenizer::token_type::array_begin: {
          auto array = _parse_array();

          if (!_root) {
            _root = array;
          }

          break;
        }
        case json_tokenizer::token_type::string: {
          auto string = _parse_string(token);

          if (!_root) {
            _root = string;
          }

          break;
        }
        case json_tokenizer::token_type::number: {
          auto number = _parse_number(token);

          if (!_root) {
            _root = number;
          }

          break;
        }
        case json_tokenizer::token_type::boolean: {
          auto boolean = _parse_boolean(token);

          if (!_root) {
            _root = boolean;
          }

          break;
        }
        case json_tokenizer::token_type::null: {
          auto null = _parse_null();

          if (!_root) {
            _root = null;
          }

          break;
        }
        default: {
          throw std::runtime_error{"Unexpected token"};
        }
      }
    }
  }

private:

  std::shared_ptr<json_node> _parse_object() {
    auto object = std::make_shared<json_object>();

    auto is_complete = false;
    auto node = std::make_shared<json_node>();

    while (!is_complete) {
      if (!_tokenizer.has_more_tokens()) {
        throw std::runtime_error{"Unexpected end of file"};
      }

      auto token = _tokenizer.next_token();

      if (token.type == json_tokenizer::token_type::object_end) {
        break;
      }

      auto key = _parse_string(token)->as_string();

      if (_tokenizer.next_token().type != json_tokenizer::token_type::colon) {
        throw std::runtime_error{"Expected colon"};
      }

      auto value_token = _tokenizer.next_token();

      switch (value_token.type) {
        case json_tokenizer::token_type::object_begin: {
          (*object)[key] = _parse_object();
          break;
        }
        case json_tokenizer::token_type::array_begin: {
          (*object)[key] = _parse_array();
          break;
        }
        case json_tokenizer::token_type::string: {
          (*object)[key] = _parse_string(value_token);
          break;
        }
        case json_tokenizer::token_type::number: {
          (*object)[key] = _parse_number(value_token);
          break;
        }
        case json_tokenizer::token_type::boolean: {
          (*object)[key] = _parse_boolean(value_token);
          break;
        }
        case json_tokenizer::token_type::null: {
          (*object)[key] = _parse_null();
          break;
        }
        default: {
          throw std::runtime_error{"Unexpected token"};
        }
      }

      if (_tokenizer.next_token().type != json_tokenizer::token_type::comma) {
        throw std::runtime_error{"Expected comma"};
      }
    }

    return std::make_shared<json_node>(object);
  }

  std::shared_ptr<json_node> _parse_array() {
    auto array = std::make_shared<json_array>();

    auto is_complete = false;
    auto node = std::make_shared<json_node>();

    while (!is_complete) {
      if (!_tokenizer.has_more_tokens()) {
        throw std::runtime_error("Unexpected end of file");
      }

      auto token = _tokenizer.next_token();

      switch (token.type) {
        case json_tokenizer::token_type::object_begin: {
          node = _parse_object();
          break;
        }
        case json_tokenizer::token_type::array_begin: {
          node = _parse_array();
          break;
        }
        case json_tokenizer::token_type::string: {
          node = _parse_string(token);
          break;
        }
        case json_tokenizer::token_type::number: {
          node = _parse_number(token);
          break;
        }
        case json_tokenizer::token_type::boolean: {
          node = _parse_boolean(token);
          break;
        }
        case json_tokenizer::token_type::null: {
          node = _parse_null();
          break;
        }
        default: {
          throw std::runtime_error("Unexpected token");
        }

        array->push_back(node);

        auto next_token = _tokenizer.next_token();

        if (next_token.type == json_tokenizer::token_type::array_end) {
          is_complete = true;
        }
      }
    }

    return std::make_shared<json_node>(array);
  }

  std::shared_ptr<json_node> _parse_string(const json_tokenizer::token& token) {
    return std::make_shared<json_node>(token.value.value());
  }

  std::shared_ptr<json_node> _parse_number(const json_tokenizer::token& token) {
    return std::make_shared<json_node>(std::stod(token.value.value()));
  }

  std::shared_ptr<json_node> _parse_boolean(const json_tokenizer::token& token) {
    return std::make_shared<json_node>(token.value.value() == "true");
  }

  std::shared_ptr<json_node> _parse_null() {
    return std::make_shared<json_node>();
  }

  json_tokenizer _tokenizer;
  std::shared_ptr<json_node> _current{};
  std::shared_ptr<json_node> _root{};

}; // class json_parser

} // namespace demo

#endif // DEMO_JSON_PARSER_HPP_
