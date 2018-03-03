#ifndef ITERATOR_H_
#define ITERATOR_H_

namespace my {

struct input_iterator_tag{};
struct output_iterator_tag{};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

template <typename T> struct iterator_traits;

template <typename T>
struct iterator_traits<T*> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using reference = T&;
  using pointer = T*;
};

template <typename T>
struct iterator_traits<const T*> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using reference = const T&;
  using pointer = const T*;
};

template <typename Iterator>
struct iterator_traits {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using diffenence_type = typename Iterator::difference_type;
  using reference = typename Iterator::reference;
  using pointer = typename Iterator::pointer;
};

template <typename T, typename Up, 
          bool = std::__has_iterator_category<iterator_traits<T>>::value>
struct has_iterator_category_convertible_to : std::integral_constant<bool, std::is_convertible<typename iterator_traits<T>::iterator_category, Up>::value> {};

template <typename T, typename Up>
struct has_iterator_category_convertible_to<T, Up, false> : std::false_type {};

template <typename T>
struct is_input_iterator 
    : public has_iterator_category_convertible_to<T, input_iterator_tag> {};

} // namespace my 

#endif // ITERATOR_H_
