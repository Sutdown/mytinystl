#ifndef MYTINYSTL_TYPE_TRAITS_H_
#define MYTINYSTL_TYPE_TRAITS_H_

// 这个头文件用于提取类型信息

// use standard header for type_traits
#include <type_traits>

namespace mystl
{
// helper struct

	/*实现了编译器常量的类型封装*/
	/*在编译期定义和使用常量值，
	并且基于这些常量进行类型判断和选择
	可以编写高效且类型安全的模板代码*/
template <class T, T v>
struct m_integral_constant
{
	/*constexpr声明编译时常量，要求变量的值在编译时确定*/
  static constexpr T value = v;
};

template <bool b>
using m_bool_constant = m_integral_constant<bool, b>;
/*类型萃取*/
typedef m_bool_constant<true>  m_true_type;
typedef m_bool_constant<false> m_false_type;

/*****************************************************************************************/
// type traits

// is_pair

// --- forward declaration begin
template <class T1, class T2>
struct pair;
// --- forward declaration end

// 通过对is_pair的特化，可以在编译器进行对mystl::pair的类型检查
// 以此做出安全的决策
template <class T>
struct is_pair : mystl::m_false_type {};

template <class T1, class T2>
struct is_pair<mystl::pair<T1, T2>> : mystl::m_true_type {};

} // namespace mystl

#endif // !MYTINYSTL_TYPE_TRAITS_H_

