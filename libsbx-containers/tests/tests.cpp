#include <gtest/gtest.h>

#include <libsbx/containers/task_graph.hpp>

auto main(int argc, char* argv[]) -> int {
  testing::InitGoogleTest(&argc, argv);

  auto task_graph = sbx::containers::task_graph{"render_graph"};

  return RUN_ALL_TESTS();
}
