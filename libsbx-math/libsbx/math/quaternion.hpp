// #ifndef LIBSBX_MATH_QUATERNION_HPP_
// #define LIBSBX_MATH_QUATERNION_HPP_

// #include <cstddef>
// #include <concepts>
// #include <cmath>
// #include <type_traits>

// #include <libsbx/math/angle.hpp>
// #include <libsbx/math/vector3.hpp>
// #include <libsbx/math/vector4.hpp>
// #include <libsbx/math/matrix4x4.hpp>

// namespace sbx::math {

// /**
//  * @brief A quaternion that represents a rotation.
//  * 
//  * @tparam Type The type of the quaternion components.
//  */
// template<std::floating_point Type>
// class basic_quaternion {

// public:

//   // -- Type aliases --

//   /** @brief Type of the quaternion components */
//   using value_type = Type;

//   /** @brief The reference type of the quaternion components. */
//   using reference = value_type&;

//   /** @brief The const reference type of the quaternion components. */
//   using const_reference = const value_type&;

//   /** @brief The pointer type of the quaternion components. */
//   using pointer = value_type*;

//   /** @brief The const pointer type of the quaternion components. */
//   using const_pointer = const value_type*;

//   /** @brief The type that can describe the length of the quaternion */
//   using length_type = Type;

//   /** @brief The type that can index components */
//   using index_type = std::size_t;

//   // -- Data members --

//   /** @brief The x-component of the quaternion. */
//   value_type x{};
//   /** @brief The y-component of the quaternion. */
//   value_type y{};
//   /** @brief The z-component of the quaternion. */
//   value_type z{};
//   /** @brief The w-component of the quaternion. */
//   value_type w{};

//   // -- Constructors --

//   /** @brief Constructs a quaternion with all components set to zero. */
//   constexpr basic_quaternion() noexcept;

//   constexpr basic_quaternion(const value_type x, const value_type y, const value_type z, const value_type w = static_cast<Type>(1)) noexcept;

//   constexpr basic_quaternion(const basic_vector3<value_type>& axis, const basic_angle<value_type>& radians) noexcept;

//   constexpr basic_quaternion(const basic_vector3<value_type>& euler_angles) noexcept;

//   constexpr basic_quaternion(const basic_quaternion&) noexcept = default;

//   template<std::floating_point Other>
//   constexpr basic_quaternion(const basic_quaternion<Other>& other) noexcept;

//   constexpr basic_quaternion(basic_quaternion&&) noexcept = default;

//   /** @brief Destroys the quaternion. */
//   ~basic_quaternion() noexcept = default;

//   static constexpr value_type dot(const basic_quaternion& lhs, const basic_quaternion& rhs) noexcept;

//   static constexpr basic_quaternion normalized(const basic_quaternion& quat) noexcept;

//   static constexpr basic_quaternion conjugated(const basic_quaternion& quat) noexcept;

//   static constexpr basic_quaternion inverted(const basic_quaternion& quat) noexcept;

//   static constexpr basic_quaternion slerp(const basic_quaternion& lhs, const basic_quaternion& rhs, const value_type factor) noexcept;

//   // -- Assignment operators --

//   constexpr basic_quaternion& operator=(const basic_quaternion&) noexcept = default;

//   template<std::floating_point Other>
//   constexpr basic_quaternion& operator=(const basic_quaternion<Other>& other) noexcept;

//   constexpr basic_quaternion& operator=(basic_quaternion&&) noexcept = default;

//   // -- Conversion operators

//   constexpr explicit operator basic_matrix4x4<value_type>() const noexcept;

//   constexpr auto to_matrix() const noexcept -> basic_matrix4x4<value_type>;

//   constexpr basic_quaternion& operator+=(const basic_quaternion& other) noexcept;

//   constexpr basic_quaternion& operator-=(const basic_quaternion& other) noexcept;

//   constexpr basic_quaternion& operator*=(const value_type scalar) noexcept;

//   constexpr basic_quaternion& operator*=(const basic_quaternion& other) noexcept;

//   constexpr basic_quaternion& operator/=(const value_type scalar);

//   // -- Access operators --

//   /**
//    * @brief Returns the component at the given index.
//    *
//    * @param index The index of the component. 
//    * 
//    * @return reference The component at the given index.
//    */
//   [[nodiscard]] constexpr reference operator[](const index_type index) noexcept;

//   /**
//    * @brief Returns the component at the given index.
//    *
//    * @param index The index of the component. 
//    * 
//    * @return reference The component at the given index.
//    */
//   [[nodiscard]] constexpr const_reference operator[](const index_type index) const noexcept;

//   // -- Data access --

//   /**
//    * @brief Return a pointer to the first component.
//    *
//    * @return pointer A pointer to the first component. 
//    */
//   [[nodiscard]] constexpr pointer data() noexcept;

//   /**
//    * @brief Return a pointer to the first component.
//    * 
//    * @return const_pointer A pointer to the first component.
//    */
//   [[nodiscard]] constexpr const_pointer data() const noexcept;

//   [[nodiscard]] constexpr length_type length() const noexcept;

//   constexpr basic_quaternion& normalize() noexcept;

//   constexpr basic_quaternion& conjugate() noexcept;

// }; // class quaternion

// // -- Free comparison operators --

// template<std::floating_point Type>
// [[nodiscard]] constexpr bool operator==(const basic_quaternion<Type>& lhs, const basic_quaternion<Type>& rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator+(basic_quaternion<Type> quat) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator-(basic_quaternion<Type> quat) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator+(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator-(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator*(basic_quaternion<Type> lhs, const Type rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator*(basic_quaternion<Type> lhs, const basic_quaternion<Type>& rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_vector3<Type> operator*(basic_quaternion<Type> lhs, const basic_vector3<Type>& rhs) noexcept;

// template<std::floating_point Type>
// [[nodiscard]] constexpr basic_quaternion<Type> operator/(basic_quaternion<Type> lhs, const Type rhs);

// // -- Type aliases --

// /** @brief Type alias for a quaternion with 32 bit floating-point components. */
// using quaternionf = basic_quaternion<std::float_t>;

// /** @brief Type alias for quaternionf */
// using quaternion = quaternionf;

// } // namespace sbx::math

// template<std::floating_point Type>
// struct std::hash<sbx::math::basic_quaternion<Type>> {
//   std::size_t operator()(const sbx::math::basic_quaternion<Type>& quaternion) const noexcept;
// }; // struct std::hash

// #include <libsbx/math/quaternion.ipp>

// #endif // LIBSBX_MATH_QUATERNION_HPP_
