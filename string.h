//www.sig.com/tech/stl/string

#include <memory>

class String {
public:
  String()
      : data_(nullptr), size_(0) {}

  String(const String& str)
      : data_(alloc_.allocate(str.size_)), size_(str.size_) {
    std::uninitialized_copy(str.data_, str.data_ + size_, data_);
  }

  String(const char* s) 
      : data_(alloc_.allocate(strlen(s))), size_(str.size_) {
    std::uninitialized_copy(s, s + strlen(s), data_);
  }

  String(size_t n, char c) 
      : data_(alloc_.allocate(n)), size_(n) {
    std::uninitialized_fill_n(data_, n, c);
  } 

  template<class Iterator>
  String(Iterator first, Iterator last)
      : data_(alloc_.allocate(last - first)), size_(last - first) {
    std::uninitialized_copy(first, last, data_);
  }

  String(String&& str) {
  
  }

  String& String(const String& str) {
    data_ = alloc_.allocate(str.size_); 
    size_ = str_.size_;
    std::uninitialized_copy(str.data_, str.data_ + size_, data_);
  }

  ~String() {
  
  }

  char* begin() const noexcept { return data_; }
  char* end() const noexcept { return data_ + size_ + 1; }

  size_t size() const noexcept { return finish_ - start_; }
  size_t length() const noexcept { return size(); }
  bool empty() const noexcept { return finish_ == start_; }

private:
  alloctor<char> alloc_;
  char* start_;
  char* finish_;
  char* end_of_storage_;
}; 
