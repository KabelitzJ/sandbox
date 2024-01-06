// #ifndef LIBSBX_MATH_VECTOR2_HPP_
// #define LIBSBX_MATH_VECTOR2_HPP_

// #include <cstddef>
// #include <cinttypes>
// #include <cmath>
// #include <concepts>
// #include <fstream>
// #include <ostream>
// #include <type_traits>

// #include <yaml-cpp/yaml.h>

// #include <fmt/format.h>

// #include <libsbx/math/concepts.hpp>

// namespace sbx::math {

// /**
//  * @brief A vector in two-dimensional space.
//  *
//  * @tparam Type The type of the vectors components.
//  */
// template<numeric Type>
// class basic_vector2 {

// public:

//   // -- Type aliases --

//   /** @brief The type of the vector components. */
//   using value_type = Type;

//   /** @brief The reference type of the vector components. */
//   using reference = value_type&;

//   /** @brief The const reference type of the vector components. */
//   using const_reference = const value_type&;

//   /** @brief The pointer type of the vector components. */
//   using pointer = value_type*;

//   /** @brief The const pointer type of the vector components. */
//   using const_pointer = const value_type*;

//   /** @brief The type that can describe the length of the vector */
//   using length_type = std::float_t;

//   /** @brief The type that can index components */
//   using index_type = std::size_t;

//   // -- Static data members --

//   /** @brief The origin of two-dimensional space */
//   inline static constexpr basic_vector2 zero{value_type{0}, value_type{0}};

//   /** @brief A unit vector along the positive x-axis */
//   inline static constexpr basic_vector2 right{value_type{-1}, value_type{0}};

//   /** @brief A unit vector along the negative x-axis */
//   inline static constexpr basic_vector2 left{value_type{1}, value_type{0}};

//   /** @brief A unit vector along the positive y-axis */
//   inline static constexpr basic_vector2 up{value_type{0}, value_type{-1}};

//   /** @brief A unit vector along the negative y-axis */
//   inline static constexpr basic_vector2 down{value_type{0}, value_type{1}};

//   // -- Data members --

//   /** @brief The x-component. */
//   value_type x{};
//   /** @brief The y-component. */
//   value_type y{};

//   // -- Constructors --

//   /** @brief Constructs a vector with all components set to zero. */
//   constexpr basic_vector2() noexcept;

//   /**
//    * @brief Constructs a vector and assigns all components to the value.
//    * 
//    * @param value Value for all components.
//    */
//   explicit constexpr basic_vector2(const value_type value) noexcept;

//   /**
//    * @brief Constructs a vector and assigns all components to the values.
//    * 
//    * @param x The value for the x component.
//    * @param y The value for the y component.
//    */
//   constexpr basic_vector2(const value_type x, const value_type y) noexcept;

//   /** 
//    * @brief Constructs a vector and copies the components from the other vector
//    *
//    * @param other The other vector to copy the components from. 
//    */
//   constexpr basic_vector2(const basic_vector2& other) noexcept = default;

//   /**
//    * @brief Constructs a vector and copies the components from the other vector
//    * 
//    * @tparam Other The type of the other vector.
//    * 
//    * @param other The other vector to copy the components from.
//    */
//   template<numeric Other>
//   explicit constexpr basic_vector2(const basic_vector2<Other>& other) noexcept;

//   /** 
//    * @brief Constructs a vector and moves the components out of the other vector
//    *
//    * @param other The other vector to move the components from. 
//    */
//   constexpr basic_vector2(basic_vector2&& other) noexcept = default;

//   /** @brief Destroys the vector */
//   constexpr ~basic_vector2() noexcept = default;

//   // -- Static member functions --

//   /**
//    * @brief Returns a normalized copy.
//    * 
//    * @return basic_vector2 The normalized vector.
//    */
//   [[nodiscard]] static constexpr auto normalized(const basic_vector2& vector) noexcept -> basic_vector2;

//   // -- Assignment operators --

//   /**
//    * @brief Copies the components from the other vector.
//    * 
//    * @param other The other vector to copy the components from.
//    * 
//    * @return basic_vector2& A reference to this vector.
//    */
//   constexpr auto operator=(const basic_vector2& other) noexcept -> basic_vector2& = default;

//   /**
//    * @brief Copies the components from the other vector.
//    * 
//    * @tparam Other The type of the other vector.
//    * 
//    * @param other The other vector to copy the components from.
//    * 
//    * @return basic_vector2& A reference to this vector.
//    */
//   template<numeric Other>
//   constexpr auto operator=(const basic_vector2<Other>& other) noexcept -> basic_vector2&;

//   /**
//    * @brief Moves the components out of the other vector.
//    * 
//    * @param other The other vector to move the components from.
//    * 
//    * @return basic_vector2& A reference to this vector.
//    */
//   constexpr auto operator=(basic_vector2&& other) noexcept -> basic_vector2& = default;

//   // -- Unary numeric operators --

//   /**
//    * @brief Negates the vector.
//    * 
//    * @return basic_vector2& A reference to this vector.
//    */
//   constexpr auto operator-() noexcept -> basic_vector2&;

//   // -- Binary numeric operators --

//   /**
//    * @brief Adds the components of the other vector to this vector.
//    * 
//    * @param other The other vector to add.
//    * 
//    * @return basic_vector2& A reference to this vector. 
//    */
//   constexpr auto operator+=(const basic_vector2& other) noexcept -> basic_vector2&;

//   template<numeric Other>
//   constexpr auto operator+=(const basic_vector2<Other>& other) noexcept -> basic_vector2&;

//   /**
//    * @brief Subtracts the components of the other vector from this vector.
//    * 
//    * @param other The other vector to subtract.
//    * 
//    * @return basic_vector2& A reference to this vector. 
//    */
//   constexpr auto operator-=(const basic_vector2& other) noexcept -> basic_vector2&;

//   /**
//    * @brief Multiplies the components of this vector by the scalar.
//    * 
//    * @param scalar The scalar to multiply by.
//    * 
//    * @return basic_vector2& A reference to this vector. 
//    */
//   constexpr auto operator*=(const value_type scalar) noexcept -> basic_vector2&;

//   template<numeric Other>
//   constexpr auto operator*=(const Other scalar) noexcept -> basic_vector2&;

//   template<numeric Other>
//   constexpr auto operator*=(const basic_vector2<Other>& other) noexcept -> basic_vector2&;

//   /**
//    * @brief Divides the components of this vector by the scalar.
//    * 
//    * @param scalar The scalar to divide by.
//    * 
//    * @throws std::domain_error If the scalar is zero.
//    * 
//    * @return basic_vector2& A reference to this vector. 
//    */
//   constexpr auto operator/=(const value_type scalar) -> basic_vector2&;

//   // -- Access operators --

//   /**
//    * @brief Returns the component at the specified index.
//    * 
//    * @param index The index of the component.
//    * 
//    * @return reference A reference to the component. 
//    */
//   [[nodiscard]] constexpr auto operator[](const index_type index) -> reference;

//   /**
//    * @brief Returns the component at the specified index.
//    * 
//    * @param index The index of the component. 
//    * 
//    * @return const_reference A const reference to the component.
//    */
//   [[nodiscard]] constexpr auto operator[](const index_type index) const -> const_reference;

//   // -- Member functions --

//   /**
//    * @brief Returns the length.
//    * 
//    * @return value_type The length.
//    */
//   [[nodiscard]] constexpr auto length() const noexcept -> length_type;

//   /** @brief Normalizes the vector. */
//   constexpr auto normalize() noexcept -> void;

//   // -- Data access --

//   /**
//    * @brief Return a pointer to the first component.
//    *
//    * @return pointer A pointer to the first component. 
//    */
//   [[nodiscard]] constexpr auto data() noexcept -> pointer;

//   /**
//    * @brief Return a pointer to the first component.
//    * 
//    * @return const_pointer A pointer to the first component.
//    */
//   [[nodiscard]] constexpr auto data() const noexcept -> const_pointer;

// }; // class basic_vector2

// // -- Free comparison operators --

// /**
//  * @brief Compares two vectors for equality.
//  * 
//  * @tparam Type The type of the vectors components.
//  * 
//  * @param lhs The left-hand side vector.
//  * @param rhs The right-hand side vector.
//  * 
//  * @return true The vectors are equal.
//  * @return false The vectors are not equal.
//  */
// template<numeric Type>
// [[nodiscard]] constexpr auto operator==(const basic_vector2<Type>& lhs, const basic_vector2<Type>& rhs) noexcept -> bool;

// // -- Free numeric operators --

// /**
//  * @brief Adds two vectors.
//  * 
//  * @tparam Type The type of the vectors components.
//  * 
//  * @param lhs The left-hand side vector.
//  * @param rhs The right-hand side vector.
//  * 
//  * @return basic_vector2<Type> The sum of the two vectors.
//  */
// template<numeric Type>
// [[nodiscard]] constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type>;

// template<numeric Type, numeric Other>
// [[nodiscard]] constexpr auto operator+(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type>;


// /**
//  * @brief Subtracts two vectors.
//  * 
//  * @tparam Type The type of the vectors components.
//  * 
//  * @param lhs The left-hand side vector.
//  * @param rhs The right-hand side vector.
//  * 
//  * @return basic_vector2<Type> The difference of the two vectors. 
//  */
// template<numeric Type>
// [[nodiscard]] constexpr auto operator-(basic_vector2<Type> lhs, const basic_vector2<Type>& rhs) noexcept -> basic_vector2<Type>;

// /**
//  * @brief Multiplies a vector by a scalar. 
//  * 
//  * @tparam Type The type of the vectors components.
//  * 
//  * @param lhs The left-hand side vector.
//  * @param rhs The right-hand side scalar.
//  * 
//  * @return basic_vector2<Type> The product of the vector and scalar. 
//  */
// template<numeric Type>
// [[nodiscard]] constexpr auto operator*(basic_vector2<Type> lhs, const Type rhs) noexcept -> basic_vector2<Type>;

// template<numeric Type, numeric Other>
// [[nodiscard]] constexpr auto operator*(basic_vector2<Type> lhs, const basic_vector2<Other>& rhs) noexcept -> basic_vector2<Type>;

// /**
//  * @brief Divides a vector by a scalar.
//  * 
//  * @tparam Type The type of the vectors components.
//  * 
//  * @param lhs The left-hand side vector.
//  * @param rhs The right-hand side scalar.
//  * 
//  * @throws std::domain_error If the scalar is zero.
//  * 
//  * @return basic_vector2<Type> The quotient of the vector and scalar.
//  */
// template<numeric Type>
// [[nodiscard]] constexpr auto operator/(basic_vector2<Type> lhs, const Type rhs) -> basic_vector2<Type>;

// // -- Type aliases --

// /** @brief Type alias for a two-dimensional vector with 32 bit floating-point components. */
// using vector2f = basic_vector2<std::float_t>;

// /** @brief Type alias for a two-dimensional vector with 32 bit integer components. */
// using vector2i = basic_vector2<std::int32_t>;

// using vector2u = basic_vector2<std::uint32_t>;

// /** @brief Type alias for vector2f. */
// using vector2 = vector2f;

// } // namespace ::math

// template<sbx::math::numeric Type>
// struct std::hash<sbx::math::basic_vector2<Type>> {
//   auto operator()(const sbx::math::basic_vector2<Type>& vector) const noexcept -> std::size_t;
// }; // struct std::hash

// template<sbx::math::numeric Type>
// struct YAML::convert<sbx::math::basic_vector2<Type>> {
//   static auto encode(const sbx::math::basic_vector2<Type>& vector) -> Node;

//   static auto decode(const Node& node, sbx::math::basic_vector2<Type>& vector) -> bool;
// }; // struct YAML::convert

// template<sbx::math::numeric Type>
// struct fmt::formatter<sbx::math::basic_vector2<Type>> {
//   template<typename ParseContext>
//   constexpr auto parse(ParseContext& context) -> decltype(context.begin());

//   template<typename FormatContext>
//   auto format(const sbx::math::basic_vector2<Type>& vector, FormatContext& context) -> decltype(context.out());
// }; // struct fmt::formatter

// #include <libsbx/math/vector2.ipp>

// #endif // LIBSBX_MATH_VECTOR2_HPP_
