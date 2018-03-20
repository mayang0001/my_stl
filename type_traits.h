#ifndef TYPE_TRAITS_
#define TYPE_TRAITS_

namespace my {

template <bool B, typename T = void>
struct enable_if {};

template <typename T>
struct enable_if<true, T> {
  using type = T;
};

} // namespace my

#endif // TYPE_TRAITS_
