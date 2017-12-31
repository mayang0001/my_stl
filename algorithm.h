#ifndef ALGORITHM_H_
#define ALGORITHM_H_

namespace my {

template <typename T>
inline const T& max(const T& a, const T& b) {
  return a < b ? b : a;
}

template <typename T, typename Functor>
inline const T& max(const T& a, const T& b, Functor compare) {
  return compare(a < b) ? b : a;
}

template <typename InputIterator, typename T>
T accumulate(InputIterator begin, InputIterator end, T init) {
  for (; begin != end; ++begin) {
    init = init + *begin;
  }
  return init;
}

template <typename InputIterator, typename T, typename Functor>
T accumulate(InputIterator begin, InputIterator end, T init, Functor binary_op) {
  for (; begin != end; ++begin) {
    init = binary_op(init, *begin);
  }
  return init;
}

template<typename InputIterator, typename T> 
InputIterator find(InputIterator begin, InputIterator end, const T& value) {
  while (begin != end && *begin != value) {
    ++begin;
  }
  return begin;
}

template<typename InputIterator, typename Function>
Function for_each(InputIterator begin, InputIterator end, Function func) {
  for (; begin != end; ++begin) {
    func(*begin);
  }
  return std::move(func);
}

template<typename InputIterator, typename OutputIterator, 
         typename UnaryOperation>
OutputIterator transform(InputIterator begin, InputIterator end,
                         OutputIterator result, UnaryOperation op) {

}

template <typename InputIterator>
typename std::iterator_traits<InputIterator>::difference_type
__Distance(InputIterator begin, InputIterator end, std::input_iterator_tag) {
  typename std::iterator_traits<InputIterator>::difference_type n = 0;
  while (begin != end) {
    ++begin;
    ++n;
  }
  return n;
}

template <typename RandomAccessIterator>
typename std::iterator_traits<RandomAccessIterator>::difference_type
__Distance(RandomAccessIterator begin, RandomAccessIterator end, std::random_access_iterator_tag) {
  return end - begin;
}

template <typename InputIterator>
typename std::iterator_traits<InputIterator>::difference_type
distance(InputIterator begin, InputIterator end) {
  typedef typename std::iterator_traits<InputIterator>::iterator_category category;
  //distance(begin, end, category);
  return __Distance(begin, end, category());
}

} // namespace my

#endif // ALGORITHM_H_

