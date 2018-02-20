#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>

template<typename T>
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

public:
  Vector() 
      : start_(nullptr), 
        finish_(nullptr), 
        end_of_storage_(nullptr) {}

  explicit Vector(int n) {
    start_ = alloc.allocate(n);
    finish_ = end_of_storage_ = start_ + n;
    for (auto p = start_; p != finish_; ) {
      alloc.construct(p++);
    }
  }

  Vector(int n, const T& val) {
    start_ = alloc.allocate(n);
    finish_ = end_of_storage_ = start_ + n;
    for (auto p = start_; p != finish_; ) {
      alloc.construct(p++, val);
    }
  }

  template<typename InputIterator>
  Vector(InputIterator first, InputIterator last) {
    auto start_and_finish = allocate_and_copy(first, last);
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector(const Vector& vec) {
    auto start_and_finish = allocate_and_copy(vec.begin(), vec.end());
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
  }

  Vector(Vector&& vec) 
      : start_(vec.start_), 
        finish_(vec.finish_), 
        end_of_storage_(vec.end_of_storage_) {
    vec.start_ = vec.finish_ = vec.end_of_storage_ = nullptr;
  }

  // Initializer list may be implemented as a pair of pointers
  // or pointer and length
  // Vector(std::initializer_list<T>& il) {
  Vector(std::initializer_list<T> il) {
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

  ~Vector() {
    free(); 
  }

  void push_back(const T& val) {
    if (finish_ == end_of_storage_) {
      reallocate();
    } 
    alloc.construct(finish_++, val);
  }

  void push_back(T&& val) {
    if (finish_ == end_of_storage_) {
      reallocate();
    }
    //alloc.construct(finish_++, val);
    alloc.construct(finish_++, std::move(val));
  }

  void pop_back() {
    alloc.destroy(--finish_); 
  } 
  
  template<typename... Args>
  iterator emplace(const_iterator position, Args&&... args) {

  }

  template<typename... Args>
  void emplace_back(Args&&... args) {
    if (finish_ == end_of_storage_) {
      reallocate();
    } 
    alloc.construct(finish_++, std::forward<Args>(args)...); 
    //alloc.construct(finish_++, args...); 
  }

  iterator insert(const_iterator position, const T& val) {
    insert(position, 1, val); 
  }

  iterator insert(const_iterator position, size_type n, const T& val) {
  
  }

  iterator insert(const_iterator position, T&& val) {
  
  }

  template <typename Iterator>
  iterator insert(const_iterator position, Iterator first, Iterator last) {
  }

  iterator erase(const_iterator position) {
    copy(position + 1, finish_, position);
    --finish_;
    alloc.destroy(finish_);
    return position;
  }

  iterator erase(const_iterator first, const_iterator last) {
    iterator i = copy(last, finish_, first);
    alloc.destroy(i, finish_);
    finish_ = finish_ - (last - first);
    return first; 
  }

  void clear() {
    erase(begin(), end());
  }

  void resize(int n) {
  }

  value_type* data() noexcept { return start_; }
  const value_type* data() const noexcept { return start_; }

  reference front() { return *start_; }
  const_reference front() const { return *start_; }
  reference back() { return *(finish_ - 1); }
  const_reference back() const { return *(finish_ - 1); }

  T& at(int idx) {
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
    std::for_each(start_, finish_, [this](T& s) { alloc.destroy(&s); });
    alloc.deallocate(start_, end_of_storage_ - start_);
  }
  
  void reallocate() {
    int old_size = size();
    int size = old_size ? old_size * 2 : 1;
    auto new_start = alloc.allocate(size);
    auto from = start_;
    auto to = new_start; 
    for (int i = 0; i < old_size; i++) {
      alloc.construct(to++, std::move(*from++));
    }
    free();
    start_ = new_start;
    finish_ = to;
    end_of_storage_ = start_ + size;
  }

  std::pair<T*, T*> allocate_and_copy(const T* begin, 
                                      const T* end) {
    auto start = alloc.allocate(end - begin);
    auto finish = std::uninitialized_copy(begin, end, start);
    return {start, finish}; 
  }

  std::allocator<T> alloc;
  T* start_;
  T* finish_;
  T* end_of_storage_;
};

#endif
