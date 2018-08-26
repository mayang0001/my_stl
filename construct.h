#ifndef CONSTRUCT_H_
#define CONSTRUCT_H_

#include "./iterator.h"
#include "./type_traits.h"

namespace my {

template <typename T>
inline void destroy(T* pointer) {
  pointer->~T();
}

template <typename ForwardIterator, typename T>
inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
  typedef typename type_traits<T>::has_trivial_destructor trivial_destructor;
  __destroy_aux(first, last, trivial_destructor());
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  __destroy(first, last, value_type(first));
}

template <typename ForwardIterator>
inline void __destroy_aux(
    ForwardIterator first, ForwardIterator last, false_type) {
  for (; first < last; ++first) {
    destroy(first);
  }
}

template <typename ForwardIterator>
inline void __destroy_aux(
    ForwardIterator first, ForwardIterator last, true_type) {}

}  // namespace my
#endif  // CONSTRUCT_H_
