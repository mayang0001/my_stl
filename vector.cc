#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>

struct Person {
  Person(const std::string n = "", int a = 0)
      : name(n), age(a) { 
    std::cout << "constructor is called" << std::endl;
  }

  Person(const Person& p)
      : name(p.name), age(p.age) { 
    std::cout << "copy constructor is called" << std::endl;
  }

  Person(Person&& p) 
      : name(std::move(p.name)), age(std::move(p.age)) {
    std::cout << "move copy constructor is called" << std::endl; 
  }

  void DebugString() {
    std::cout << "person name is " << name << std::endl;
    std::cout << "person age is " << age << std::endl;
  }

  std::string name;
  int age;
};

template<typename T>
class Vector {
  friend void swap(Vector& x, Vector& y);

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
    auto start_and_finish = allocate_and_copy(vec.begin(), vec.end());
    free();
    start_ = start_and_finish.first;
    finish_ = end_of_storage_ = start_and_finish.second;
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
  void emplace_back(Args&&... args) {
    if (finish_ == end_of_storage_) {
      reallocate();
    } 
    alloc.construct(finish_++, args...); 
  }

  void clear() {
    std::for_each(start_, finish_, [this](T& s) { alloc.destroy(&s); });
    finish_ = start_;
  }

  void resize(int n) {
  }

  T front() { return *start_; }
  T back() { return *(finish_ - 1); }

  T& operator[] (int idx) { return *(start_ + idx); }
    
  T* begin() noexcept { return start_; }
  T* end() noexcept { return finish_; }

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

  std::pair<T*, T*> allocate_and_copy(const T* b, const T* e) {
    auto start = alloc.allocate(e - b);
    auto finish = std::uninitialized_copy(b, e, start);
    return {start, finish}; 
  }

  std::allocator<T> alloc;
  T* start_;
  T* finish_;
  T* end_of_storage_;
};

template<typename T>
void swap(Vector<T>& x, Vector<T>& y) {
  using std::swap;
  swap(x.start_, y.start_);
  swap(x.finish_, y.finish_);
  swap(x.end_of_storage_, y.end_of_storage_);
}

int main() {
  Vector<Person> vec1 = {{"mayang", 18}, {"mayang", 19}};
  Vector<Person> vec;
  vec.emplace_back("mayang", 20);
  vec.push_back(std::move(Person("mayang", 20)));
  for (auto v : vec1) v.DebugString();
}



