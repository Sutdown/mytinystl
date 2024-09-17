#ifndef MYTINYSTL_ITERATOR_H_
#define MYTINYSTL_ITERATOR_H_

// 这个头文件用于迭代器设计，包含了一些模板结构体与全局函数，

#include <cstddef>

#include "type_traits.h"

namespace mystl
{

// 迭代器种类，iterator_category
    /*提前设计便于编译，如果在运行期再选择哪种迭代器，会影响程序效率，
    最好能在编译器就选择正确的版本*/
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

// iterator 模板，也就是associate type
/*五个内嵌相应型别*/
template <class Category, class T, class Distance = ptrdiff_t,
  class Pointer = T*, class Reference = T&>
  struct iterator
{
  typedef Category       iterator_category;
  typedef T                    value_type;
  typedef Pointer          pointer;
  typedef Reference      reference;
  typedef Distance        difference_type;
};

// iterator traits
 /*has_iterator_cat：检测某个类型是否具有iterator_category成员，这个技术称为SFINAE
    可以在编译时检测类型的特性*/
  /*has_iterator_cat类利用SFINAE规则，使编译器根据迭代器有/没有iterator_catefory类型成员，
  分别送入两个test函数，这两个test函数并不会执行，
  而是在编译期间，通过test函数的返回值判断迭代器进了哪个函数，
  从而判断迭代器有没有iterator_category类*/
template <class T>
struct has_iterator_cat
{
private:
  struct two { char a; char b; };
  template <class U> static two test(...);
  template <class U> static char test(typename U::iterator_category* = 0);
public:
  static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

/*traits：
  * 通过模板提取类型的某些属性或行为，以实现高度灵活和可扩展的代码。
  * 可以在编译时检查，操作和使用类型的相关信息。
  * 核心思想在于将类型信息和行为封装到独立的模板类中，为泛型算法提供特定的类型信息或者功能。
  */
template <class Iterator, bool>
struct iterator_traits_impl {};
/*提取用户自定义类型的相关信息*/
template <class Iterator>
struct iterator_traits_impl<Iterator, true>
{
  typedef typename Iterator::iterator_category iterator_category;
  typedef typename Iterator::value_type        value_type;
  typedef typename Iterator::pointer           pointer;
  typedef typename Iterator::reference         reference;
  typedef typename Iterator::difference_type   difference_type;
};

/* iterator_traits_helper：进行迭代器类型萃取，主要依赖于偏特化和SFINAE技术*/
template <class Iterator, bool>
struct iterator_traits_helper {};

template <class Iterator>
struct iterator_traits_helper<Iterator, true>
  : public iterator_traits_impl<Iterator,
  std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
  std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value>
{
};

// 萃取迭代器的特性
template <class Iterator>
struct iterator_traits 
  : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

// 针对原生指针的偏特化版本
template <class T>
struct iterator_traits<T*>
{
  typedef random_access_iterator_tag           iterator_category;
  typedef T                                    value_type;
  typedef T*                                   pointer;
  typedef T&                                   reference;
  typedef ptrdiff_t                            difference_type;
};

template <class T>
struct iterator_traits<const T*>
{
  typedef random_access_iterator_tag           iterator_category;
  typedef T                                    value_type;
  typedef const T*                             pointer;
  typedef const T&                             reference;
  typedef ptrdiff_t                            difference_type;
};

// 判断一个迭代器可否被视为输入迭代器、输出迭代器、双向迭代器、或者随机访问迭代器。
// 编译时确立迭代器类型
template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
struct has_iterator_cat_of
  : public m_bool_constant<std::is_convertible<
  typename iterator_traits<T>::iterator_category, U>::value>
{
};

// 萃取某种迭代器
template <class T, class U>
struct has_iterator_cat_of<T, U, false> : public m_false_type {};

template <class Iter>
struct is_input_iterator : public has_iterator_cat_of<Iter, input_iterator_tag> {};

template <class Iter>
struct is_output_iterator : public has_iterator_cat_of<Iter, output_iterator_tag> {};

template <class Iter>
struct is_forward_iterator : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

template <class Iter>
struct is_bidirectional_iterator : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

template <class Iter>
struct is_random_access_iterator : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

template <class Iterator>
struct is_iterator :
  public m_bool_constant<is_input_iterator<Iterator>::value ||
    is_output_iterator<Iterator>::value>
{
};

// 萃取某个迭代器的 category
template <class Iterator>
typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&)
{
  typedef typename iterator_traits<Iterator>::iterator_category Category;
  return Category();
}

// 萃取某个迭代器的 distance_type
template <class Iterator>
typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&)
{
  return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// 萃取某个迭代器的 value_type
template <class Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&)
{
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}


// 以下函数用于计算迭代器间的距离

// distance 的 input_iterator_tag 的版本
template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag)
{
  typename iterator_traits<InputIterator>::difference_type n = 0;
  while (first != last)
  {
    ++first;
    ++n;
  }
  return n;
}

// distance 的 random_access_iterator_tag 的版本
template <class RandomIter>
typename iterator_traits<RandomIter>::difference_type
distance_dispatch(RandomIter first, RandomIter last,
                  random_access_iterator_tag)
{
  return last - first;
}

template <class InputIterator>
typename iterator_traits<InputIterator>::difference_type
distance(InputIterator first, InputIterator last)
{
  return distance_dispatch(first, last, iterator_category(first));
}

// 以下函数用于让迭代器前进 n 个距离
/*每个advance的最后一个参数都只声明性别，不指定参数名称
   因为它纯粹用来激活重载机制*/
// advance 的 input_iterator_tag 的版本
template <class InputIterator, class Distance>
void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag)
{
  while (n--) 
    ++i;
}

// advance 的 bidirectional_iterator_tag 的版本
template <class BidirectionalIterator, class Distance>
void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag)
{
  if (n >= 0)
    while (n--)  ++i;
  else
    while (n++)  --i;
}

// advance 的 random_access_iterator_tag 的版本
template <class RandomIter, class Distance>
void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag)
{
  i += n;
}

/*这是一个对外开放的上层控制接口，调用上述各个重载的advance_batch*/
template <class InputIterator, class Distance>
void advance(InputIterator& i, Distance n)
{
  advance_dispatch(i, n, iterator_category(i));
}

/*****************************************************************************************/

// 模板类 : reverse_iterator
// 代表反向迭代器，使前进为后退，后退为前进
template <class Iterator>
class reverse_iterator
{
private:
  Iterator current;  // 记录对应的正向迭代器

public:
  // 反向迭代器的五种相应型别
  typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
  typedef typename iterator_traits<Iterator>::value_type        value_type;
  typedef typename iterator_traits<Iterator>::difference_type   difference_type;
  typedef typename iterator_traits<Iterator>::pointer           pointer;
  typedef typename iterator_traits<Iterator>::reference         reference;

  typedef Iterator                                              iterator_type;
  typedef reverse_iterator<Iterator>                            self;

public:
  // 构造函数
  reverse_iterator() {}
  explicit reverse_iterator(iterator_type i) :current(i) {}
  reverse_iterator(const self& rhs) :current(rhs.current) {}

public:
  // 取出对应的正向迭代器
  iterator_type base() const 
  { return current; }

  // 重载操作符
  reference operator*() const
  { // 实际对应正向迭代器的前一个位置
    auto tmp = current;
    return *--tmp;
  }
  pointer operator->() const
  {
    return &(operator*());
  }

  // 前进(++)变为后退(--)
  self& operator++()
  {
    --current;
    return *this;
  }
  self operator++(int)
  {
    self tmp = *this;
    --current;
    return tmp;
  }
  // 后退(--)变为前进(++)
  self& operator--()
  {
    ++current;
    return *this;
  }
  self operator--(int)
  {
    self tmp = *this;
    ++current;
    return tmp;
  }

  self& operator+=(difference_type n)
  {
    current -= n;
    return *this;
  }
  self operator+(difference_type n) const
  {
    return self(current - n);
  }
  self& operator-=(difference_type n)
  {
    current += n;
    return *this;
  }
  self operator-(difference_type n) const
  {
    return self(current + n);
  }

  reference operator[](difference_type n) const
  {
    return *(*this + n);
  }
};

// 重载 operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs)
{
  return rhs.base() - lhs.base();
}

// 重载比较操作符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
  const reverse_iterator<Iterator>& rhs)
{
  return rhs.base() < lhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return !(lhs == rhs);
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
               const reverse_iterator<Iterator>& rhs)
{
  return rhs < lhs;
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return !(rhs < lhs);
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs)
{
  return !(lhs < rhs);
}

} // namespace mystl

#endif // !MYTINYSTL_ITERATOR_H_

