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

template <typename Iterator, typename T>
T accumulate(Iterator begin, Iterator end, T init) {
  for (; begin != end; ++begin) {
    init = init + *begin;
  }
  return init;
}

template <typename Iterator, typename T, typename Fucntor>
T accumulate(Iterator begin, Iterator end, T init, Functor binary_op) {
  for (; begin != end; ++begin) {
    init = binary_op(init, *begin);
  }
  return init;
}

template<typename Iterator, typename T> 
Iterator find(Iterator begin, Iterator end, const T& value) {
  while (begin != end && *begin != value) {
    ++begin;
  }
  return begin;
}

template<typename Iterator, typename Function>
Function for_each(Iterator begin, Iterator end, Function func) {
  for (Iterator iter = begin; iter != end; ++iter) {
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
