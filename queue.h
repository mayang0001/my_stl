#ifndef QUEUE_H_
#define QUEUE_H_

#include <queue>

template <typename T, typename Container = std::deque<T>>
class Queue {
 public:
  using value_type = typename Container::value_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using size_type = typename Container::size_type;
  using container_type = Container;

  explicit Queue(const container_type& container) {
    container_ = container;
  }
  explicit Queue(container_type&& container = container_type()) {
    container_ = std::move(container);
  }

  bool Empty() { return container_.empty(); }
  size_type Size() { return container_.size(); }

  void Pop() { container_.pop_front(); }
  void Push(const value_type& val) { container_.push_back(val); }
  void Push(value_type&& val) { container_.push_back(std::move(val)); }
  template <typename... Args>
  void Emplace(Args&&... args) {
    container_.emplace_back(std::forward<Args>(args)...);
  }

  reference Front() { return container_.front(); }
  const_reference Front() const { return container_.front(); }
  reference Back() { return container_.back(); }
  const_reference Back() const { return container_.back(); }
 private:
  container_type container_;
};


// Functions for priority_queue

template <typename RandomAccessIterator>
void PushHeap(RandomAccessIterator first, RandomAccessIterator last) {
  PushHeap(first, (last - first - 1), 0, *(last - 1));
}

template <typename RandomAccessIterator, typename Distance, typename T>
void PushHeap(RandomAccessIterator first, Distance hole_idx, Distance top_idx, 
              const T& value) {
  Distance parent = (hole_idx - 1) / 2;
  while (hole_idx > top_idx && *(first + parent) < value) {
    *(first + hole_idx) = value;
    hole_idx = parent;
    parent = (hole_idx - 1) / 2;
  }
  *(first + hole_idx) = value;
}

template <typename RandomAccessIterator>
void PopHeap(RandomAccessIterator first, RandomAccessIterator last) {
  PopHeap(first, 0, last - first - 1, *(last - 1));
}

template <typename RandomAccessIterator, typename T>
void PopHeap(RandomAccessIterator first, Distance hole_idx, Distance len,
             const T& value) {
  *(first + len) = *first;

  // set to right child
  Distance child = hole_idx * 2 + 2;
  while (child < len) {
    if (*(first + child) < *(first + (child - 1))) {
      --child; 
    }
  
    if (value > *(first + child)) {
      break;
    } else {
      *(first + hole_idx) = *(first + child);
    }
    hole_idx = child;
    child = hole_idx * 2 + 2;
  }
  *(first + hole_idx) = value;
}

template <typename T, typename Container = std::vector<T>, 
          typename Compare = std::less<T>>
class PriorityQueue {
 public:
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using reference = typename Container::reference;
  using const_reference = typename Container::const_reference;
  using container_type = Container;

  explicit PriorityQueue(const container_type& container) {
    container_ = container;
  }
  explicit PriorityQueue(container_type&& container = container_type()) {
    container_ = std::move(container);
  }

  bool Empty() const { return container_.empty(); }
  size_type Size() const { return container_.size(); }

  reference Top() { return container_.front(); }
  const_reference Top() const { return container_.front(); }

  void Push(const value_type& value) {
    container_.push_back(value);
    PushHeap(container_.begin(), container_.end());
  }
  void Push(value_type&& value) {
    container_.push_back(std::move(value));
    PushHeap(container_.begin(), container_.end());
  }
  template <typename... Args>
  void Emplace(Args&&... args) {
    container_.emplace_back(std::forward<Args>(args)...);
    PushHeap(container_.begin(), container_.end());
  }

  void Pop() {
    PopHeap(container_.begin(), container_.end());
    container_.PopBack();
  }

 private:
  container_type container_;
  Compare comp_;
};
#endif // QUEUE_H_
