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
 * @file libsbx/ecs/entity.cpp
 */

#include <libsbx/ecs/entity.hpp>

#include <utility>

namespace sbx::ecs {

const entity entity::null{id_mask, version_mask};

entity::entity(entity&& other) noexcept
: _value{std::exchange(other._value, null._value)} { }

entity& entity::operator=(entity&& other) noexcept {
  _value = std::exchange(other._value, null._value);

  return *this;
}

entity::entity() noexcept
: _value{null._value} { }

entity::entity(const id_type id, const version_type version) noexcept
: _value{(id << version_shift) | version } { }

entity::id_type entity::_id() const noexcept {
  return _value >> version_shift;
}

entity::version_type entity::_version() const noexcept {
  return _value & version_mask;
}

void entity::_increment_version() noexcept {
  _value = (_value & id_mask) | ((_value + 1) & version_mask);
}

bool operator==(const entity& lhs, const entity& rhs) noexcept {
  return lhs._value == rhs._value;
}

std::ostream& operator<<(std::ostream& output_stream, const entity& entity) {
  return output_stream << "(id: " << entity._id() << ", version: " << entity._version() << ')';
  // return output_stream << std::bitset<sizeof(entity::value_type) * 4>{entity._value};
}

} // namespace sbx::ecs

std::size_t std::hash<sbx::ecs::entity>::operator()(const sbx::ecs::entity& entity) const noexcept {
  return std::hash<sbx::ecs::entity::value_type>{}(entity._value);
} 
