#ifndef ALLOC_H_
#define ALLOC_H_

namespace my{

template <typename T, typename Alloc>
class simple_alloc {
 public:
  static T* allocate(size_t n) {
    return (T*) Alloc::allocate(sizeof(T) * n);
  }

  static T* allocate() {
    return (T*) Alloc::allocate(sizeof(T));
  }

  static void deallocate(T* p, size_t n) {
    Alloc::deallocate(p, sizeof(T) * n);
  }

  static void deallocate(T* p) {
    Alloc::deallocate(p, sizeof(T));
  }
};

template <int inst>
class malloc_alloc_template {
 public:
  static void* allocate(size_t n) {
    void* result = malloc(n);
    if (result == nullptr) {
      result = oom_malloc(n);
    }
    return result;
  }

  static void* deallocate(void* p, size_t /* n */) {
    free(p);
  }

 private:
  static void* oom_malloc(size_t) {}
  static void* oom_realloc(void* , size_t) {}
};

typedef malloc_alloc_template<0> alloc;

} // end namespace my

#endif // ALLOC_H_
