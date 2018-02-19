#ifndef STACK_H_
#define STACK_H_

#include <queue>

template <typename T, typename Container = std::deque<T>>
class Stack {
 public:
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using size_type = typename Container::size_type;
  using container_type = Container;

  explicit Stack(const container_type& container) {
    container_ = container;
  }
  explicit Stack(container_type&& container = container_type()) {
    container_ = std::move(container);
  }

  size_type Size() const { return container_.size(); }
  bool Empty() const { return container_.empty(); }

  void Push(const T& val) { container_.push_back(val); }
  void Push(T&& val) { container_.push_back(std::move(val)); }
  template <typename... Args>
  void Emplace(Args&&... args) {
    container_.emplace_back(std::forward<Args>(args)...);
  }

  void Pop() { container_.pop_back(); }

  reference Top() { return container_.back(); }
  const_reference Top() const { return container_.back(); } 
 private:
  Container container_;
};

#endif // STACK_H_
