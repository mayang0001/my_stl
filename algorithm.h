#ifndef ITERATOR_H_
#define ITERATOR_H_

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

template <typename InputIterator, typename T, typename Fucntor>
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
    func(*iter);
  }
  return std::move(func);
}

template<typename InputIterator, typename OutputIterator, 
         typename UnaryOperation>
OutputIterator transform(InputIterator begin, InputIterator end,
                         outputIterator result, UnaryOperation op) {

}

#endif
