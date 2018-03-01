#ifndef LIST_H_
#define LIST_H_

#include <memory>
#include <iostream>
#include <iterator>

namespace my {

template <typename T>
struct ListNode {
  using pointer = ListNode*;
  pointer next;
  pointer prev;
  T data;
};

template <typename T, typename Ref, typename Ptr>
struct ListIterator {
  using iterator = ListIterator<T, T&, T*>;
  using const_iterator = ListIterator<T, const T&, const T*>;
  using Self = ListIterator<T, Ref, Ptr>;

  using value_type = T;
  using reference = Ref;
  using pointer = Ptr;
  using difference_type = ptrdiff_t;
  using size_type = size_t;
  using iterator_category = std::bidirectional_iterator_tag; 

  using Node = ListNode<T>;

  Node* node;
  
  ListIterator() { node = nullptr; }
  ListIterator(Node* x) { node = x; }
  ListIterator(const ListIterator& x) { node = x.node; }

  //const_iterator& operator=(const iterator& iter) {
  //  return const_iterator(iter.node); 
  //}

  bool operator==(const ListIterator& x) { return node == x.node; }
  bool operator!=(const ListIterator& x) { return node != x.node; }

  reference operator*() const { return node->data; }
  pointer operator->() const { return &(node->data); }

  Self& operator++() {
    node = node->next;
    return *this;
  }

  Self operator++(int) {
    Self temp = *this;
    ++*this;
    return temp;
  }

  Self& operator--() {
    node = node->prev;
    return *this;
  }

  Self operator--(int) {
    Self temp = *this;
    --*this;
    return temp;
  }
};

template <typename T, typename Alloc>
struct AllocTraits {
  using allocator_type = typename Alloc::template rebind<T>::other;
};

template <typename T, typename Alloc = std::allocator<T>>
class List {
public:
  using value_type = T;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using iterator = ListIterator<value_type, reference, pointer>; 
  using const_iterator = ListIterator<value_type, const_reference, const_pointer>; 
private:
  using Node = ListNode<T>;
  using allocator_type = 
      typename AllocTraits<Node, Alloc>::allocator_type;

public:

  explicit List(const allocator_type& alloc = allocator_type()) 
      : alloc_(alloc) {
    InitializeEmpty(); 
  }

  explicit List(size_type n) {
    InitializeEmpty();
    for ( ; n > 0; --n) {
      Insert(Begin(), value_type());
    } 
  }

  List(size_type n, const value_type& val,
       const allocator_type& alloc = allocator_type()) 
     : alloc_(alloc) {
    InitializeEmpty();
    for (; n > 0; --n) {
      Insert(Begin(), n, val);
    }
  }

  template <typename InputIterator>
  List(InputIterator first, InputIterator last,
       const allocator_type& alloc = allocator_type()) 
      : alloc_(alloc) {
    InitializeEmpty();
    for(; first != last; ++first) {
      Insert(End(), *first);
    }
  }

  List(const List& list) {
    InitializeEmpty();
    for (auto iter = list.Begin(); iter != list.End(); ++iter) {
      Insert(Begin(), *iter);
    } 
  }

  List(const List& list, const allocator_type& alloc) 
      : alloc_(alloc) {
    InitializeEmpty();
    for (auto iter = list.Begin(); iter != list.End(); ++iter) {
      Insert(Begin(), *iter);
    } 
  }

  List(List&& list) {
  
  }

  List(List&& list, const allocator_type& alloc) {
  
  }

  //List(List&& list, const allocator_type& alloc = allocator_type()) {
  //
  //}

  List(std::initializer_list<value_type> il, 
       const allocator_type& alloc = allocator_type()) 
      : alloc_(alloc) {
    InitializeEmpty();
    Insert(Begin(), il.begin(), il.end()); 
  }

  List& operator=(const List& list) {
  
  }

  List& operator=(List&& list) {
    if (node_ != list.node_) {
      Clear();
      node_ = list.node_;
      list.InitializeEmpty();
    }  
    return *this;
  }

  List& operator=(std::initializer_list<value_type> il) {
  
  }

  void push_back(const T& val) {
    Insert(End(), val);
  }
  
  void push_back(T&& val) {
    Insert(End(), std::move(val));
  }
  
  template <typename... Args>
  void emplace_back(Args&&... args) {
    Insert(End(), std::forward<Args>(args)...); 
  }

  template <typename... Args>
  void emplace_front(Args&& ...args) {
    Insert(Begin(), std::forward<Args>(args)...); 
  }

  void push_front(const T& val) {
    Insert(Begin(), val);
  }

  void push_front(T&& val) {
    Insert(Begin(), std::move(val));
  }

  void pop_back() {
    // erase(--End());
    iterator tmp = End();
    erase(--tmp);
  }

  void pop_front() {
    erase(Begin()); 
  }

  void Clear() {
    auto iter = Begin();
    while (iter != End()) {
      iter = erase(iter);
    }
  }

  //iterator Insert(const_iterator position, const value_type& val) {
  iterator Insert(iterator position, const value_type& val) {
    Node* tmp = CreateNode(val);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return iterator(tmp);
  }

  //iterator Insert(const_iterator position, size_type n, const T& val) {
  iterator Insert(iterator position, size_type n, const T& val) {
    for (size_type i = 0; i < n; ++i) {
      position = Insert(position, val);
    }
    return position;
  }

  template <typename InputIterator>
  //iterator Insert(const_iterator position, InputIterator first, 
  iterator Insert(iterator position, InputIterator first, 
                  InputIterator last) {
    for (; first != last; ++first) {
      position = Insert(position, *first);
    } 
    return position;
  }

  //iterator Insert(const_iterator position, value_type&& val) {
  iterator Insert(iterator position, value_type&& val) {
    Node* tmp = CreateNode(std::move(val));
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }
  
  //iterator Insert(const_iterator position, 
  iterator Insert(iterator position, 
                  std::initializer_list<value_type> il) {
    return Insert(position, il.begin(), il.end());
  }

  //template <typename... Args>
  //iterator Insert(const_iterator position, Args&&...  args) {
  //  Node* tmp = CreateNode(std::forward<Args>(args)...);
  //  tmp->next = position.node;
  //  tmp->prev = position.node->prev;
  //  position.node->prev->next = tmp;
  //  position.node->prev = tmp;
  //}

  iterator erase(iterator position) {
    Node* next_node = position.node->next;
    Node* prev_node = position.node->prev;
    next_node->prev = prev_node;
    prev_node->next = next_node;
    destroy_node(position.node);
    return next_node;
  }

  iterator erase(iterator first, iterator last) {
    auto iter = first;
    while (iter != last) {
      iter = erase(iter);
    }
    return iter;
  }

  iterator Begin() { return node_->next; }
  const_iterator Begin() const { return node_->next; }
  const_iterator Cbegin() const { return node_->next; }
  iterator End() { return node_; }
  const_iterator End() const { return node_; }
  const_iterator Cend() const { return node_; }

  bool Empty() const { return node_ == node_->next; }
  size_type Size() const {
    size_type result = std::distance(Begin(), End());
    return result;  
  }

  reference Front() { return *Begin(); }
  const_reference Front() const { return *Begin(); }
  reference Back() { return *(--End()); }
  const_reference Back() const { return *(--End()); }

  void Remove(const value_type& val) {}
  template <typename Predicate>
  void RemoveIf(Predicate prde) {}
  void Unique() {}
  template <typename BinaryPredict>
  void Unique(BinaryPredict binary_pred) {}
  void merge(List& x);
  void merge(List&& x);
  template <typename Compare>
  void Merge(List& x, Compare comp);
  template <typename Compare>
  void Merge(List&& x, Compare comp);
  void Sort();
  template <typename Compare>
  void Sort(Compare comp);
  void Reverse() {}

private:
  void InitializeEmpty() {
    Node* tmp = CreateNode();
    node_->next = tmp;
    node_->prev = tmp;
  }

  Node* CreateNode(const value_type& val = value_type()) {
    Node* tmp = alloc_.allocate(1);
    alloc_.construct(&tmp->data, val); 
    return tmp;
  }

  Node* CreateNode(value_type&& val) {
    Node* tmp = alloc_.allocate(1);
    alloc_.construct(&tmp->data, std::move(val)); 
    return tmp;
  }

  template <typename... Args>
  Node* CreateNode(Args&&... args) {
    Node* tmp = alloc_.allocate(1);
    alloc_.construct(&tmp->data, std::forward<Args>(args)...);
  }

  void DestroyNode(Node* p) {
    alloc_.destroy(p);
    alloc_.deallocate(p, 1);
  }

  allocator_type alloc_;
  Node* node_; 
};

} // namespace my

#endif // LIST_H_
