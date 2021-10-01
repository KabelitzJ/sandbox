#include <gtest/gtest.h>

bool test() {
  return true;
}

int main(int, char**) {
  return test() ? 0 : 1;
}