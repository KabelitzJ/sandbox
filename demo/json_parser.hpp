#ifndef DEMO_JSON_PARSER_HPP_
#define DEMO_JSON_PARSER_HPP_

#include <memory>
#include <string>
#include <filesystem>

#include "json_node.hpp"
#include "json_tokenizer.hpp"

namespace demo {

class json_parser {

public:

  json_parser(const std::filesystem::path& path)
  : _tokenizer{path},
    _current{} { }

  ~json_parser() = default;

  std::shared_ptr<json_node> parse() {
    auto root = std::shared_ptr<json_node>{};

    while (_tokenizer.has_more_tokens()) {
      auto token = _tokenizer.next_token();

      switch (token.type) {
        case json_tokenizer::token_type::object_begin: {
          auto object = _parse_object();

          if (!root) {
            root = object;
          }

          break;
        }
        case json_tokenizer::token_type::array_begin: {
          auto array = _parse_array();

          if (!root) {
            root = array;
          }

          break;
        }
        case json_tokenizer::token_type::string: {
          auto string = _parse_string(token);

          if (!root) {
            root = string;
          }

          break;
        }
        case json_tokenizer::token_type::number: {
          auto number = _parse_number(token);

          if (!root) {
            root = number;
          }

          break;
        }
        case json_tokenizer::token_type::boolean: {
          auto boolean = _parse_boolean(token);

          if (!root) {
            root = boolean;
          }

          break;
        }
        case json_tokenizer::token_type::null: {
          auto null = _parse_null();

          if (!root) {
            root = null;
          }

          break;
        }
        default: {
          throw std::runtime_error{"Unexpected token"};
        }
      }
    }

    return root;
  }

private:

  std::shared_ptr<json_node> _parse_object() {
    auto object = std::make_shared<json_object>();

    auto is_complete = false;

    while (!is_complete) {
      auto key_token = _tokenizer.next_token();

      if (key_token.type != json_tokenizer::token_type::string) {
        throw std::runtime_error{"Expected string"};
      }

      auto key = _parse_string(key_token)->as_string();

      if (_tokenizer.next_token().type != json_tokenizer::token_type::colon) {
        throw std::runtime_error{"Expected colon"};
      }

      auto value_token = _tokenizer.next_token();

      switch (value_token.type) {
        case json_tokenizer::token_type::object_begin: {
          auto value = _parse_object();

          object->insert(std::make_pair(key, value));

          break;
        }
        case json_tokenizer::token_type::array_begin: {
          auto value = _parse_array();

          object->insert(std::make_pair(key, value));

          break;
        }
        case json_tokenizer::token_type::string: {
          auto value = _parse_string(value_token);

          object->insert(std::make_pair(key, value));

          break;
        }
        case json_tokenizer::token_type::number: {
          auto value = _parse_number(value_token);

          object->insert(std::make_pair(key, value));

          break;
        }
        case json_tokenizer::token_type::boolean: {
          auto value = _parse_boolean(value_token);

          object->insert(std::make_pair(key, value));

          break;
        }
        case json_tokenizer::token_type::null: {
          auto value = _parse_null();

          object->insert(std::make_pair(key, value));

          break;
        }
        default: {
          throw std::runtime_error{"Unexpected token"};
        }
      }

      auto end_token = _tokenizer.next_token();

      if (end_token.type == json_tokenizer::token_type::comma) {
        continue;
      } else if (end_token.type == json_tokenizer::token_type::object_end) {
        is_complete = true;
      } else {
        throw std::runtime_error{"Unexpected token"}; 
      }
    }

    return std::make_shared<json_node>(object);
  }

  std::shared_ptr<json_node> _parse_array() {
    auto array = std::make_shared<json_array>();

    auto is_complete = false;

    while (!is_complete) {
      if (!_tokenizer.has_more_tokens()) {
        throw std::runtime_error("Unexpected end of file");
      }

      auto token = _tokenizer.next_token();

      switch (token.type) {
        case json_tokenizer::token_type::object_begin: {
          auto value = _parse_object();
          array->push_back(value);
          break;
        }
        case json_tokenizer::token_type::array_begin: {
          auto value = _parse_array();
          array->push_back(value);
          break;
        }
        case json_tokenizer::token_type::string: {
          auto value = _parse_string(token);
          array->push_back(value);
          break;
        }
        case json_tokenizer::token_type::number: {
          auto value = _parse_number(token);
          array->push_back(value);
          break;
        }
        case json_tokenizer::token_type::boolean: {
          auto value = _parse_boolean(token);
          array->push_back(value);
          break;
        }
        case json_tokenizer::token_type::null: {
          auto value = _parse_null();
          array->push_back(value);
          break;
        }
        default: {
          throw std::runtime_error("Unexpected token");
        }
      }

      auto end_token = _tokenizer.next_token();

      if (end_token.type == json_tokenizer::token_type::comma) {
        continue;
      } else if (end_token.type == json_tokenizer::token_type::array_end) {
        is_complete = true;
      } else {
        throw std::runtime_error{"Unexpected token"}; 
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
    const auto value = bool{token.value.value() == "true"};
    return std::make_shared<json_node>(value);
  }

  std::shared_ptr<json_node> _parse_null() {
    return std::make_shared<json_node>();
  }

  json_tokenizer _tokenizer;
  std::shared_ptr<json_node> _current{};

}; // class json_parser

} // namespace demo

#endif // DEMO_JSON_PARSER_HPP_
