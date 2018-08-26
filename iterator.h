#ifndef ITERATOR_H_
#define ITERATOR_H_

namespace my {

struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename Iter, bool> struct iterator_traits_impl {};

template <typename Iterator>
struct iterator_traits_impl<Iterator, true> {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using reference = typename Iterator::reference;
  using pointer = typename Iterator::pointer;
};

template <typename Iterator, bool> struct iterator_traits_ {};

template <typename Iterator>
struct iterator_traits_<Iterator, true>
    : iterator_traits_impl<
          Iterator,
          std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
          std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};

template <typename Iterator>
struct iterator_traits : iterator_traits_<Iterator, std::__has_iterator_category<Iterator>::value> {};

template <typename T>
struct iterator_traits<T*> {
  using iterator_category = random_access_iterator_tag;
  using value_type = typename std::remove_const<T>::type;
  using difference_type = ptrdiff_t;
  using reference = T&;
  using pointer = T*;
};

template <typename Iterator>
typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
  return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <typename T, typename Up, 
          bool = std::__has_iterator_category<iterator_traits<T>>::value>
struct has_iterator_category_convertible_to : public std::integral_constant<bool, std::is_convertible<typename iterator_traits<T>::iterator_category, Up>::value> {};

template <typename T, typename Up>
struct has_iterator_category_convertible_to<T, Up, false> : std::false_type {};

template <typename T>
struct is_input_iterator 
    : public has_iterator_category_convertible_to<T, input_iterator_tag> {};

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
__Distance(InputIterator begin, InputIterator end, input_iterator_tag) {
  typename iterator_traits<InputIterator>::difference_type n = 0;
  while (begin != end) {
    ++begin;
    ++n;
  }
  return n;
}

template <typename RandomAccessIterator>
typename iterator_traits<RandomAccessIterator>::difference_type
__Distance(RandomAccessIterator begin, RandomAccessIterator end, random_access_iterator_tag) {
  return end - begin;
}

template <typename InputIterator>
typename iterator_traits<InputIterator>::difference_type
Distance(InputIterator begin, InputIterator end) {
  typedef typename iterator_traits<InputIterator>::iterator_category category;
  return __Distance(begin, end, category());
}

} // namespace my 

#endif // ITERATOR_H_
