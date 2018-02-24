#ifndef PAIR_H_
#define PAIR_H_

template <typename T1, typename T2>
struct Pair {
  using first_type = T1;
  using second_type = T2;

  Pair() {};

  template <typename U, typename V>
  Pair(const Pair<U, V>& p)
      : first(p.first), second(p.second) {}
  template <typename U, typename V>
  Pair(Pair<U, V>&& p)
      : first(std::move(p.first)), second(std::move(p.second)) {}
  Pair(const Pair& p) = default;
  Pair(Pair&& p) = default;

  // TODO Why This
  Pair(const first_type& a, const second_type& b)
      : first(a), second(b) {}
  template <typename U, typename V>
  Pair(U&& a, V&& b)
      : first(std::move(a)), second(std::move(b)) { std::cout << "lll" << std::endl; }
  
  // Why dont use = default
  //Pair& operator=(const Pair& p) = default;
  template <typename U, typename V>
  Pair& operator=(const Pair<U, V>& p) {
    if (this != &p) {
      first = p.first;
      second = p.second;
    }
    return *this;
  }
  template <typename U, typename V>
  Pair& operator=(Pair<U, V>&& p) {
    if (this != &p) {
      first = std::move(p.first);
      second = std::move(p.second);
    }
    return *this;
  }

  first_type first;
  second_type second;
};

template <typename T1, typename T2>
Pair<T1, T2> MakePair(T1&& a, T2&& b) {
//Pair<V1, V2> MakePair(T1&& a, T2&& b) {
  return Pair<T1, T2>(std::move(a), std::move(b));
}

#endif
