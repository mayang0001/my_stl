#ifndef TUPLE_H_
#define TUPLE_H_

#include <cstddef>

using std::size_t;

// Definitions for Tuple struct
template <typename... Ttypes> struct Tuple{};

template <>
struct Tuple<> {
};

template <typename Tfirst, typename... Trest>
struct Tuple<Tfirst, Trest...> : public Tuple<Trest...> {
  Tfirst value;
};


// Definitions for Tuple element
template <size_t idx, typename Ttuple> struct TupleElement;

template <typename Tfirst, typename... Trest>
struct TupleElement<0, Tuple<Tfirst, Trest...>> {
  using Tvalue = Tfirst;
  using Ttuple = Tuple<Tfirst, Trest...>;
};

template <size_t idx, typename Tfirst, typename... Trest>
struct TupleElement<idx, Tuple<Tfirst, Trest...>> 
    : public TupleElement<idx - 1, Tuple<Trest...>> {};


// Definitions for Get
template <size_t idx, typename... Ttypes>
typename TupleElement<idx, Tuple<Ttypes...>>::Tvalue&
Get(Tuple<Ttypes...>& tuple) {
  using Ttuple = typename TupleElement<idx, Tuple<Ttypes...>>::Ttuple;
  return static_cast<Ttuple&>(tuple).value;
}


// Definitions for MakeTuple
template <size_t idx, typename Ttuple>
void FillTuple(Ttuple& to_fill) {}

template <size_t idx, typename Ttuple, typename Tfirst, typename... Trest>
void FillTuple(Ttuple& to_fill, const Tfirst& first, Trest... rest) {
  Get<idx>(to_fill) = first;
  FillTuple<idx + 1>(to_fill, rest...);
}

template <typename... Ttypes>
Tuple<Ttypes...> MakeTuple(Ttypes... args) {
  Tuple<Ttypes...> result;
  FillTuple<0>(result, args...);
  return result;
}

#endif // TUPLE_H_
