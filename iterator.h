template <typename Iterator>
struct iterator_traits {

};

template <typename T>
struct iterator_traits<T*> {

};

template <typename T>
struct iterator_traits<const T*> {

};
