#ifndef HASH_FUNC_H_
#define HASH_FUNC_H_

template <typename T>
struct Hash {};

template <>
struct Hash<int> {
  size_t operator()(int val) {
    return val;
  }
};

#endif // HASH_FUNC_H_
