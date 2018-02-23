template <typename Iterator>
struct iterator_traits {
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using diffenence_type = typename Iterator::difference_type;
  using reference = typename Iterator::reference;
  using pointer = typename Iterator::pointer;
};

template <typename T>
struct iterator_traits<T*> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = otfdiff_t;
  using reference = T&;
  using pointer = T*;
};

template <typename T>
struct iterator_traits<const T*> {
  using iterator_category = random_access_iterator_tag;
  using value_type = T;
  using difference_type = otfdiff_t;
  using reference = const T&;
  using pointer = const T*;
};
