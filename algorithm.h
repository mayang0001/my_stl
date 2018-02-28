#ifndef ALGORITHM_H_
#define ALGORITHM_H_

namespace my {

template <typename T>
inline const T& Max(const T& a, const T& b) {
  return a < b ? b : a;
}

template <typename T, typename Functor>
inline const T& Max(const T& a, const T& b, Functor compare) {
  return compare(a < b) ? b : a;
}

template <typename InputIterator, typename T>
T Accumulate(InputIterator begin, InputIterator end, T init) {
  for (; begin != end; ++begin) {
    init = init + *begin;
  }
  return init;
}

template <typename InputIterator, typename T, typename Functor>
T Accumulate(InputIterator begin, InputIterator end, T init, Functor binary_op) {
  for (; begin != end; ++begin) {
    init = binary_op(init, *begin);
  }
  return init;
}

template<typename InputIterator, typename T> 
InputIterator Find(InputIterator begin, InputIterator end, const T& value) {
  while (begin != end && *begin != value) {
    ++begin;
  }
  return begin;
}

template<typename InputIterator, typename Function>
Function ForEach(InputIterator begin, InputIterator end, Function func) {
  for (; begin != end; ++begin) {
    func(*begin);
  }
  return std::move(func);
}

template<typename InputIterator, typename OutputIterator, 
         typename UnaryOperation>
OutputIterator Transform(InputIterator begin, InputIterator end,
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
Distance(InputIterator begin, InputIterator end) {
  typedef typename std::iterator_traits<InputIterator>::iterator_category category;
  //distance(begin, end, category);
  return __Distance(begin, end, category());
}

template <typename InputIterator, typename Distance>
void __Advance(InputIterator& i, Distance n, std::input_iterator_tag) {
  while (n--) {
    ++i;
  }
}

template <typename BidirectionalIterator, typename Distance>
void __Advance(BidirectionalIterator& i, Distance n, std::bidirectional_iterator_tag) {
  if (n >= 0) {
    while (n--) {
      ++i;
    }
  } else {
    while (n++) {
      --i;
    }
  }
}

template <typename RandomAccessIterator, typename Distance>
void __Advance(RandomAccessIterator& i, Distance n, std::random_access_iterator_tag) {
  i += n;
}

template <typename InputIterator, typename Distance>
void Advance(InputIterator& i, Distance n) {
  typedef typename std::iterator_traits<InputIterator>::iterator_category category;
  return __Advance(i, n, category());
}

} // namespace my

#endif // ALGORITHM_H_

