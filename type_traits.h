#ifndef TYPE_TRAITS_H_
#define TYPE_TRAITS_H_

namespace my {

struct false_type {};
struct true_type {};

template <typename T>
struct type_traits {
  typedef false_type has_trivial_default_constructor;
  typedef false_type has_trivial_copy_constructor;
  typedef false_type has_trivial_assignment_operator;
  typedef false_type has_trivial_destructor;
  typedef false_type is_POD_type;
};

template <>
struct type_traits<char> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<signed char> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned char> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<short> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned short> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<int> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned int> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<long> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned long> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<float> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <>
struct type_traits<double> {
  typedef true_type has_trivial_default_constructor;
  typedef true_type has_trivial_copy_constructor;
  typedef true_type has_trivial_assignment_operator;
  typedef true_type has_trivial_destructor;
  typedef true_type is_POD_type;
};

template <bool B, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
  using type = T;
};

}  // namespace my

#endif  // TYPE_TRAITS_H_
