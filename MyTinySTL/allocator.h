#ifndef MYTINYSTL_ALLOCATOR_H_
#define MYTINYSTL_ALLOCATOR_H_

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

#include "construct.h"
#include "util.h"

namespace mystl
{

// 模板类：allocator
// 模板函数代表数据类型
template <class T>
class allocator
{
public:
    /*allocator的必要接口，第三章详细阐述*/
  typedef T            value_type;
  typedef T*           pointer;
  typedef const T*     const_pointer;
  typedef T&           reference;
  typedef const T&     const_reference;
  typedef size_t       size_type;
  typedef ptrdiff_t    difference_type;

public:
    /*执行内存分配和释放*/
    /*静态成员函数属于类而不是类的对象实例，便于使用时不需要先创建实例
       可以直接调用静态函数。*/
    /*静态成员函数可以编译时进行优化，不依赖与对象的动态状态，可以提供
       性能上的优势。*/
  static T*   allocate();
  static T*   allocate(size_type n);

  static void deallocate(T* ptr);
  static void deallocate(T* ptr, size_type n);

  static void construct(T* ptr);
  static void construct(T* ptr, const T& value);
  static void construct(T* ptr, T&& value);

  template <class... Args>
  static void construct(T* ptr, Args&& ...args);

  /*destory的两个版本*/
  static void destroy(T* ptr);
  static void destroy(T* first, T* last);
};

template <class T>
T* allocator<T>::allocate()
{
  return static_cast<T*>(::operator new(sizeof(T)));
}

template <class T>
T* allocator<T>::allocate(size_type n)
{
  if (n == 0)
    return nullptr;
  return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <class T>
void allocator<T>::deallocate(T* ptr)
{
  if (ptr == nullptr)
    return;
  ::operator delete(ptr);
}

template <class T>
void allocator<T>::deallocate(T* ptr, size_type /*size*/)
{
  if (ptr == nullptr)
    return;
  ::operator delete(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr)
{
  mystl::construct(ptr);
}

template <class T>
void allocator<T>::construct(T* ptr, const T& value)
{
  mystl::construct(ptr, value);
}

template <class T>
 void allocator<T>::construct(T* ptr, T&& value)
{
     /*T&&是右值引用，表示找打一个可以移动的对象，窃取临时对象的资源
        move是一种显示的移动语义，可以安全的将value的值转移，不需要额外拷贝*/
  mystl::construct(ptr, mystl::move(value));
}

template <class T>
template <class ...Args>
 void allocator<T>::construct(T* ptr, Args&& ...args)
{
     // 可变模板参数的通用引用，不确定是左值还是右值
     /*完美转发，保留参数的值类别（左值或右值），
        从而确保在调用目标函数时能正确地传递原始参数的语义。*/
  mystl::construct(ptr, mystl::forward<Args>(args)...);
}

template <class T>
void allocator<T>::destroy(T* ptr)
{
  mystl::destroy(ptr);
}

template <class T>
void allocator<T>::destroy(T* first, T* last)
{
  mystl::destroy(first, last);
}

} // namespace mystl
#endif // !MYTINYSTL_ALLOCATOR_H_

