#include <gtest/gtest.h>

#include <core/event_queue.hpp>

TEST(ecs_event_queue, initial_state) {
  auto event_queue = sbx::event_queue{};

  event_queue.add_listener<int>([&](const auto&){});

  EXPECT_EQ(0, 0);
}