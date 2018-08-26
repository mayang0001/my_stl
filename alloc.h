#ifndef ALLOC_H_
#define ALLOC_H_

namespace my {

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

  static void deallocate(void* p, size_t /* n */) {
    free(p);
  }

 private:
  static void* oom_malloc(size_t) { return nullptr; }
  static void* oom_realloc(void* , size_t) { return nullptr; }
};

typedef malloc_alloc_template<0> malloc_alloc;

enum { ALIGN = 8 };
enum { MAX_BYTES = 128 };
enum { NUM_FREE_LISTS = MAX_BYTES / ALIGN };

template <bool threads, int inst>
class default_alloc_template {
 public:
  static void* allocate(size_t n);
  static void deallocate(void* p, size_t n);

 private:
  static size_t RoundUp(size_t bytes) {
    return (bytes + ALIGN - 1) & ~(ALIGN - 1);
  }
  static int FreeListIdx(size_t bytes) {
    return (bytes + ALIGN - 1) / ALIGN - 1;
  }
  static void* ReFill(size_t bytes);
  static char* chunk_alloc(size_t bytes, int& n);

  union obj {
    union obj* free_list_link;
  };

  static obj* free_list[NUM_FREE_LISTS];
  static char* start_free;
  static char* end_free;
};

template <bool threads, int inst>
char* default_alloc_template<threads, inst>::start_free = nullptr;
template <bool threads, int inst>
char* default_alloc_template<threads, inst>::end_free = nullptr;

template <bool threads, int inst>
void* default_alloc_template<threads, inst>::allocate(size_t n) {
  if (n > MAX_BYTES) {
    return malloc_alloc::allocate(n);
  }

  obj* my_free_list = free_list + FreeListIdx(n);
  obj* result = *my_free_list;
  if (result == nullptr) {
    return ReFill(RoundUp(n));
  }
  *my_free_list = result->free_list_link;
  return result;
}

template <bool threads, int inst>
void default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
  if (n > MAX_BYTES) {
    malloc_alloc::deallocate(p, n);
  }
}

template <bool threads, int inst>
void* default_alloc_template<threads, inst>::ReFill(size_t bytes) {
  int n = 20;
  char* chunk = chunk_alloc(bytes, n);

  if (n == 1) {
    return chunk;
  }

  obj* result = (obj*)chunk;
  obj* my_free_list = free_list + FreeListIdx(bytes);

  *my_free_list = next_obj = (obj*)(chunk + n);
  for (int i = 1; ; ++i) {
    current_obj = next_obj;
    next_obj = current_obj + bytes;
    if (i == n - 1) {
      current_obj->free_list_link = nullptr;
      break;
    } else {
      current_obj->free_list_link = next_obj;
    }
  }
  return result;
}

template <bool threads, int inst>
char* default_alloc_template<threads, inst>::chunk_alloc(size_t bytes, int& n) {
  char* result;
  size_t total_bytes = size * n;
  size_t bytes_left = end_free - start_free;

  if (bytes_left >= total_bytes) {
    result = start_free;
    start_free += total_bytes;
    return result;
  } else if (bytes_left >= bytes) {
    n = bytes_left / bytes;
    total_bytes = bytes * n;
    result = start_free;
    start_free += total_bytes;
    return result;
  } else {
    if (bytes_left > 0) {
      obj* my_free_list = free_list + FreeListIdx(bytes_left);
      start_free->free_list_link = my_free_list;
      *my_free_list = start_free;
    }

    size_t bytes_to_get = total_bytes * 2 + RoundUp(heap_size >> 4);
    start_free = (char*) malloc(bytes_to_get);
    if (start_free == nullptr) {
      for (int i = size; i < MAX_BYTES; i += ALIGN) {
        my_free_list = free_list + FreeListIdx(i);
        p = *my_free_list;
        if (p == nullptr) {
          *my_free_list = p->free_list_link;
          start_free = (char*)p;
          end_free = start_free + i;
          return chunk_alloc(bytes, n);
        }
      }
      end_free = 0;
      start_free = (char*)malloc_alloc::allocate(bytes_to_get);
    }
    heap_size += bytes_to_get;
    end_free = start_free + bytes_to_get;
    return chunk_alloc(size, n);
  }
}

typedef malloc_alloc_template<0> alloc;

}  // end namespace my

#endif  // ALLOC_H_
