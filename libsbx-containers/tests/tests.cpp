#include <gtest/gtest.h>

#include <libsbx/containers/task_graph.hpp>

auto main(int argc, char* argv[]) -> int {
  testing::InitGoogleTest(&argc, argv);

  auto task_graph = sbx::containers::task_graph{"render_graph"};

  auto [a, b, c] = task_graph.emplace(
    [](){},
    [](){},
    [](){}
  );

  a.precede(b, c);

  c.succeed(b);

  return RUN_ALL_TESTS();
}
