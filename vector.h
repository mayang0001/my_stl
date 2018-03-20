#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include "algorithm.h"
#include "type_traits.h"

namespace my {

template<typename T, typename Alloc = std::allocator<T>>
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

public:
  explicit Vector(const allocator_type& alloc = allocator_type()) 
      : start_(nullptr), 
        finish_(nullptr), 
        end_of_storage_(nullptr),
        alloc_(alloc) {}

  explicit Vector(size_type n) {
    start_ = alloc_.allocate(n);
    finish_ = end_of_storage_ = start_ + n;
    for (auto p = start_; p != finish_; ) {
      alloc_.construct(p++);
    }
  }

  Vector(size_type n, const value_type& val,
         const allocator_type& alloc = allocator_type())
      : alloc_(alloc) {
    start_ = alloc_.allocate(n);
    finish_ = end_of_storage_ = start_ + n;
    for (auto p = start_; p != finish_; ) {
      alloc_.construct(p++, val);
    }
  }

  template<typename InputIterator>
  Vector(InputIterator first, InputIterator last,
         const allocator_type& alloc = allocator_type(), 
         typename enable_if<is_input_iterator<InputIterator>::value>::type* = 0) {
    auto start_and_finish = allocate_and_copy(first, last);
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector(const Vector& vec) {
    auto start_and_finish = allocate_and_copy(vec.begin(), vec.end());
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector(const Vector& vec, const allocator_type& alloc) 
      : alloc_(alloc) {
    auto start_and_finish = allocate_and_copy(vec.begin(), vec.end());
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector(Vector&& vec) 
      : start_(vec.start_), 
        finish_(vec.finish_), 
        end_of_storage_(vec.end_of_storage_),
        alloc_(vec.alloc_) {
    vec.start_ = vec.finish_ = vec.end_of_storage_ = nullptr;
  }

  // Initializer list may be implemented as a pair of pointers
  // or pointer and length
  // Vector(std::initializer_list<value_type>& il) {
  Vector(std::initializer_list<value_type> il,
         const allocator_type& alloc = allocator_type()) 
      : alloc_(alloc) {
    auto start_and_finish = allocate_and_copy(il.begin(), il.end());
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector& operator=(const Vector& vec) {
    if (this != &vec) {
      auto start_and_finish = allocate_and_copy(vec.begin(), vec.end());
      free();
      start_ = start_and_finish.first;
      finish_ = end_of_storage_ = start_and_finish.second;
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
    alloc_.construct(finish_++, val);
  }

  void push_back(value_type&& val) {
    if (finish_ == end_of_storage_) {
      reallocate();
    }
    //alloc_.construct(finish_++, val);
    alloc_.construct(finish_++, std::move(val));
  }

  void pop_back() {
    alloc_.destroy(--finish_); 
  } 
  
  template<typename... Args>
  iterator emplace(const_iterator position, Args&&... args) {

  }

  template<typename... Args>
  void emplace_back(Args&&... args) {
    if (finish_ == end_of_storage_) {
      reallocate();
    } 
    alloc_.construct(finish_++, std::forward<Args>(args)...); 
    //alloc.construct(finish_++, args...); 
  }

  iterator insert(const_iterator position, const value_type& val) {
  }

  iterator insert(const_iterator position, size_type n, const value_type& val) {
    if (size_type(end_of_storage_ - finish_) < n) {
      const size_type old_size = end_of_storage_ - start_;
      const size_type new_size = old_size + Max(old_size, n);

      iterator new_start = alloc_.allocate(new_size);
      iterator new_finish = new_start;
      try {
        new_finish = std::uninitialized_copy(start_, position, new_start);
        new_finish = std::uninitialized_fill_n(new_finish, n, val);
        new_finish = std::uninitialized_copy(position, finish_, new_finish);
      } catch(...) {
        destroy(new_start, new_finish);
        alloc_.deallocate(new_start, new_size);
        throw;
      }
      destroy(start_, finish_);
      alloc_.deallocate(start_, end_of_storage_);
      start_ = new_start;
      finish_ = new_finish;
      end_of_storage_ = new_start + new_size;
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
  iterator insert(const_iterator position, Iterator first, Iterator last, 
                  typename enable_if<std::__is_input_iterator<Iterator>::value>::type* = 0) {
    std::cout << "template insert is called" << std::endl;
  }

  iterator Erase(const_iterator position) {
    std::copy(position + 1, finish_, position);
    --finish_;
    alloc_.destroy(finish_);
    return position;
  }

  iterator Erase(const_iterator first, const_iterator last) {
    iterator i = std::copy(last, finish_, first);
    alloc_.destroy(i, finish_);
    finish_ = finish_ - (last - first);
    return first; 
  }

  void clear() {
    Erase(begin(), end());
  }

  void resize(int n) {
  }

  pointer data() noexcept { return start_; }
  const_pointer data() const noexcept { return start_; }

  reference front() { return *start_; }
  const_reference front() const { return *start_; }
  reference back() { return *(finish_ - 1); }
  const_reference back() const { return *(finish_ - 1); }

  reference at(int idx) {
    // TODO check(idx < size());
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
    std::for_each(start_, finish_, [this](value_type& s) { alloc_.destroy(&s); });
    alloc_.deallocate(start_, end_of_storage_ - start_);
  }
  
  void reallocate() {
    int old_size = size();
    int size = old_size ? old_size * 2 : 1;
    auto new_start = alloc_.allocate(size);
    auto from = start_;
    auto to = new_start; 
    for (int i = 0; i < old_size; i++) {
      alloc_.construct(to++, std::move(*from++));
    }
    free();
    start_ = new_start;
    finish_ = to;
    end_of_storage_ = start_ + size;
  }

  std::pair<value_type*, value_type*> allocate_and_copy(const_pointer begin,
                                                        const_pointer end) {
    auto start = alloc_.allocate(end - begin);
    auto finish = std::uninitialized_copy(begin, end, start);
    return {start, finish}; 
  }

  allocator_type alloc_;
  pointer start_;
  pointer finish_;
  pointer end_of_storage_;
};

} // namespace my

#endif // VECTOR_H_
