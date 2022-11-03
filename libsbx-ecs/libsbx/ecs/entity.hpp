/* 
 * Copyright (c) 2022 Jonas Kabelitz
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * You should have received a copy of the MIT License along with this program.
 * If not, see <https://opensource.org/licenses/MIT/>.
 */

/**
 * @file libsbx/ecs/entity.hpp
 */

#ifndef LIBSBX_ECS_ENTITY_HPP_
#define LIBSBX_ECS_ENTITY_HPP_

#include <bitset>
#include <cinttypes>
#include <iostream>

namespace sbx::ecs {

/**
 * @ingroup libsbx-ecs
 */

class entity final {

  friend struct std::hash<entity>;

  friend class registry;

  friend bool operator==(const entity& lhs, const entity& rhs) noexcept;
  friend std::ostream& operator<<(std::ostream& output_stream, const entity& entity);

  using id_type = std::uint32_t;
  using version_type = std::uint16_t;
  using value_type = std::uint32_t;

  inline static constexpr auto id_mask = value_type{0xFFFFF};
  inline static constexpr auto version_mask = value_type{0xFFF};
  inline static constexpr auto version_shift = std::size_t{12}; 

public:

  static const entity null;

  entity(const entity& other) noexcept = default;

  entity(entity&& other) noexcept;

  ~entity() noexcept = default;

  entity& operator=(const entity& other) noexcept = default;

  entity& operator=(entity&& other) noexcept;

private:

  entity() noexcept;

  entity(const id_type id, const version_type version) noexcept;

  id_type _id() const noexcept;

  version_type _version() const noexcept;

  void _increment_version() noexcept;

  value_type _value{};

}; // class entity

bool operator==(const entity& lhs, const entity& rhs) noexcept;

std::ostream& operator<<(std::ostream& output_stream, const entity& entity);

} // namespace sbx::ecs

template<>
struct std::hash<sbx::ecs::entity> {
  std::size_t operator()(const sbx::ecs::entity& entity) const noexcept;
}; // struct std::hash

#endif // LIBSBX_ECS_ENTITY_HPP_
