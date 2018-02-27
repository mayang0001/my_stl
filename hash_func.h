#ifndef HASH_FUNC_H_
#define HASH_FUNC_H_

template <typename T>
struct Hash {};

template <>
struct Hash<char> {
  size_t operator()(char val) {
    return val;
  }
};

template <>
struct Hash<unsigned char> {
  size_t operator()(unsigned char val) {
    return val;
  }
};

template <>
struct Hash<short> {
  size_t operator()(short val) {
    return val;
  }
};

template <>
struct Hash<unsigned short> {
  size_t operator()(unsigned short val) {
    return val;
  }
};

template <>
struct Hash<int> {
  size_t operator()(int val) {
    return val;
  }
};

template <>
struct Hash<unsigned int> {
  size_t operator()(unsigned int val) {
    return val;
  }
};

template <>
struct Hash<long> {
  size_t operator()(long val) {
    return val;
  }
};

template <>
struct Hash<unsigned long> {
  size_t operator()(unsigned long val) {
    return val;
  }
};
#endif // HASH_FUNC_H_
