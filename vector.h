#ifndef VECTOR_H_
#define VECTOR_H_

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "./algorithm.h"
#include "./type_traits.h"
#include "./alloc.h"

namespace my {

template <typename T>
inline void destroy(T* pointer) {
  pointer->~T();
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  for (; first < last; ++first) {
    destroy(first);
  }
}

template<typename T, typename Alloc = alloc>
class Vector {
 public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = value_type*;
  using const_iterator = const value_type*;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using allocator_type = Alloc;

  using data_allocator = simple_alloc<value_type, Alloc>;

 public:
  explicit Vector(const allocator_type& alloc = allocator_type())
      : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}

  explicit Vector(size_type n)
      : start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {
    start_ = data_allocator::allocate(n);
    finish_ = std::uninitialized_fill_n(start_, n, value_type());
    end_of_storage_ = finish_;
  }

  Vector(size_type n, const value_type& val,
         const allocator_type& alloc = allocator_type()) {
    start_ = data_allocator::allocate(n);
    finish_ = std::uninitialized_fill_n(start_, n, val);
    end_of_storage_ = finish_;
  }

  template<typename InputIterator>
  Vector(InputIterator first, InputIterator last,
         const allocator_type& alloc = allocator_type()) {
         // typename
         //     enable_if<is_input_iterator<InputIterator>::value>::type* = 0) {
    size_type n = std::distance(first, last);
    start_ = data_allocator::allocate(n);
    end_of_storage_ = start_ + n;
    finish_ = std::uninitialized_copy(first, last, start_);
  }

  Vector(const Vector& vec) {
    start_ = data_allocator::allocate(vec.size());
    end_of_storage_ = start_ + vec.size();
    finish_ = std::uninitialized_copy(vec.begin(), vec.end(), start_);
  }

  Vector(const Vector& vec, const allocator_type& alloc) {
    start_ = data_allocator::allocate(vec.size());
    end_of_storage_ = start_ + vec.size();
    finish_ = uninitialized_copy(vec.begin(), vec.end(), start_);
  }

  Vector(Vector&& vec)
      : start_(vec.start_),
        finish_(vec.finish_),
        end_of_storage_(vec.end_of_storage_) {
    vec.start_ = vec.finish_ = vec.end_of_storage_ = nullptr;
  }

  // Initializer list may be implemented as a pair of pointers
  // or pointer and length
  // Vector(std::initializer_list<value_type>& il) {
  Vector(std::initializer_list<value_type> il,
         const allocator_type& alloc = allocator_type()) {
    size_type n = std::distance(il.begin(), il.end());
    start_ = data_allocator::allocate(n);
    end_of_storage_ = start_ + n;
    finish_ = std::uninitialized_copy(il.begin(), il.end(), start_);
  }

  Vector& operator=(const Vector& vec) {
    if (this != &vec) {
      free();
      size_type n = 0;
      distance(vec.begin(), vec.end(), n);
      start_ = data_allocator::allocate(n);
      end_of_storage_ = start_ + n;
      finish_ = uninitialized_copy(start_, finish_, start_);
    }
    return *this;
  }

  Vector& operator=(Vector&& vec) {
    if (this != &vec) {
      free();
      start_ = vec.start_;
      finish_ = vec.finish_;
      end_of_storage_ = vec.end_of_storage_;

      vec.start_ = vec.finish_ = vec.end_of_storage_ = nullptr;
    }
    return *this;
  }

  Vector& operator=(std::initializer_list<T> il) {
  }

  ~Vector() {
    free();
  }

  void push_back(const value_type& val) {
    if (finish_ == end_of_storage_) {
      reallocate();
    }
    construct(finish_++, val);
  }

  void push_back(value_type&& val) {
    if (finish_ == end_of_storage_) {
      reallocate();
    }
    construct(finish_++, std::move(val));
  }

  void pop_back() {
    destroy(--finish_);
  }

  template<typename... Args>
  iterator emplace(const_iterator position, Args&&... args) {
  }

  template<typename... Args>
  void emplace_back(Args&&... args) {
    if (finish_ == end_of_storage_) {
      reallocate();
    }
    construct(finish_++, std::forward<Args>(args)...);
    // alloc.construct(finish_++, args...);
  }

  iterator insert(iterator position, const value_type& val) {
    insert(position, 1, val);
  }

  iterator insert(iterator position, size_type n, const value_type& val) {
    if (size_type(end_of_storage_ - finish_) < n) {
      const size_type old_size = end_of_storage_ - start_;
      const size_type new_size = old_size + Max(old_size, n);

      iterator new_start = data_allocator::allocate(new_size);
      iterator new_finish = new_start;
      iterator ret_position;
      try {
        new_finish = std::uninitialized_copy(start_, position, new_start);
        ret_position = new_finish;
        new_finish = std::uninitialized_fill_n(new_finish, n, val);
        new_finish = std::uninitialized_copy(position, finish_, new_finish);
      } catch(...) {
        destroy(new_start, new_finish);
        data_allocator::deallocate(new_start, new_size);
        throw;
      }
      destroy(start_, finish_);
      data_allocator::deallocate(start_, end_of_storage_ - start_);
      start_ = new_start;
      finish_ = new_finish;
      end_of_storage_ = new_start + new_size;
      return ret_position;
    } else {
      const size_type num_elems = finish_ - position;
      iterator old_finish = finish_;
      finish_ += n;

      std::uninitialized_copy(position, finish_, finish_ - num_elems);
      std::uninitialized_fill_n(position, n, val);
      return position + n;
    }
  }

  iterator insert(const_iterator position, value_type&& val) {
  }

  template <typename Iterator>
  iterator insert(
      const_iterator position, Iterator first, Iterator last,
      typename
      enable_if<std::__is_input_iterator<Iterator>::value>::type* = 0) {
  }

  iterator erase(const_iterator position) {
    std::copy(position + 1, finish_, position);
    --finish_;
    destroy(finish_);
    return position;
  }

  iterator erase(iterator first, iterator last) {
    iterator i = std::copy(last, finish_, first);
    destroy(i, finish_);
    finish_ = finish_ - (last - first);
    return first;
  }

  void clear() {
    erase(begin(), end());
  }


  void resize(size_t n, const value_type& val) {
    if (n < size()) {
      erase(begin() + n, end());
    } else {
      insert(end(), n - size(), val);
    }
  }
  void resize(int n) {
    resize(n, value_type());
  }

  pointer data() noexcept { return start_; }
  const_pointer data() const noexcept { return start_; }

  reference front() { return *start_; }
  const_reference front() const { return *start_; }
  reference back() { return *(finish_ - 1); }
  const_reference back() const { return *(finish_ - 1); }

  reference at(int idx) {
    // TODO(may) check(idx < size());
    return *(start_ + idx);
  }

  reference operator[](int idx) { return *(start_ + idx); }
  const_reference operator[](int idx) const { return *(start_ + idx); }

  iterator begin() noexcept { return start_; }
  const_iterator begin() const noexcept { return start_; }
  const_iterator cbegin() const noexcept { return start_; }
  iterator end() noexcept { return finish_; }
  const_iterator end() const noexcept { return finish_; }
  const_iterator cend() const noexcept { return finish_; }

  int size() const noexcept { return finish_ - start_; }
  int capacity() const noexcept { return end_of_storage_ - start_; }
  bool empty() const noexcept { return finish_ == start_; }

 private:
  void free() {
    destroy(start_, finish_);
    data_allocator::deallocate(start_, end_of_storage_ - start_);
  }

  void reallocate() {
    int old_size = size();
    int size = old_size ? old_size * 2 : 1;
    auto new_start = data_allocator::allocate(size);
    auto from = start_;
    auto to = new_start;
    for (int i = 0; i < old_size; i++) {
      construct(to++, std::move(*from++));
    }
    free();
    start_ = new_start;
    finish_ = to;
    end_of_storage_ = start_ + size;
  }

  pointer start_;
  pointer finish_;
  pointer end_of_storage_;
};

}  // namespace my

#endif  // VECTOR_H_
