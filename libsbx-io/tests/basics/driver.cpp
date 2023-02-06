#include <iostream>
#include <regex>

#include <gtest/gtest.h>

#include <libsbx/io/io.hpp>

TEST(libsbx_io, empty_node) {
  auto node = sbx::io::node{};

  EXPECT_EQ(node.to_string(), "");
}

TEST(libsbx_io, signed_integer_node) {
  auto node = sbx::io::node{std::int32_t{42}};

  EXPECT_EQ(std::regex_replace(node.to_string(), std::regex{"\n"}, ""), std::string{"42"});
}

TEST(libsbx_io, unsigned_integer_node) {
  auto node = sbx::io::node{std::uint32_t{42}};

  EXPECT_EQ(std::regex_replace(node.to_string(), std::regex{"\n"}, ""), std::string{"42"});
}

TEST(libsbx_io, floating_point_node) {
  auto node = sbx::io::node{std::float_t{0.69}};

  EXPECT_EQ(std::regex_replace(node.to_string(), std::regex{"\n"}, ""), std::string{"0.69"});
}

TEST(libsbx_io, boolean_node) {
  auto node = sbx::io::node{false};

  EXPECT_EQ(std::regex_replace(node.to_string(), std::regex{"\n"}, ""), std::string{"false"});
}

TEST(libsbx_io, string_node) {
  auto node1 = sbx::io::node{"from_literal"};
  auto node2 = sbx::io::node{std::string{"from_string"}};

  EXPECT_EQ(std::regex_replace(node1.to_string(), std::regex{"\n"}, ""), std::string{"\"from_literal\""});
  EXPECT_EQ(std::regex_replace(node2.to_string(), std::regex{"\n"}, ""), std::string{"\"from_string\""});
}

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
