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

  bool operator==(const ListIterator& x) { return node == x.node; }
  bool operator!=(const ListIterator& x) { return node != x.node; }

  T& operator*() const { return node->data; }
  T* operator->() const { return &(node->data); }

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

template <typename T, typename Alloc = std::allocator>
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
  using allocator_type = Alloc<Node>;

public:

  explicit List(const allocator_type& alloc = allocate_type()) { 
    InitializeEmpty(); 
  }

  explicit List(size_type n) {
  
  }

  List(size_type n, const value_type& val, i
       const allocator_type& alloc = allocate_type()) {
  
  }

  List(std::initializer_list<T> il) {
    InitializeEmpty();
    for (auto i : il) {
      insert(end(), i);
    }
  }

  template <typename InputIterator>
  List(InputIterator first, InputIterator last,
       const allocator_type& alloc = allocate_type()) {
    InitializeEmpty();
    auto iter = first;
    while (iter != last) {
      insert(end(), *iter);
      ++iter;
    }
  }

  List(const List& list) {
    InitializeEmpty();
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      insert(end(), *iter);
    } 
  }

  List(const List& list, const allocator_type& alloc = allocate_type()) {
    InitializeEmpty();
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      insert(end(), *iter);
    } 
  }

  List(List&& list) {
  
  }

  List(List&& list, const allocator_type& alloc) {
  
  }

  //List(List&& list, const allocator_type& alloc = allocate_type()) {
  //
  //}

  List& operator=(const List& list) {
  
  }

  List& operator=(List&& list) {
    if (node != list.node) {
      Clear();
      node = list.node;
      list.InitializeEmpty();
    }  
    return *this;
  }

  List& operator=(std::initialize_list<value_type> il) {
  
  }

  void push_back(const T& val) {
    insert(end(), val);
  }
  
  void push_back(T&& val) {
    insert(end(), std::move(val));
  }
  
  template <typename... Args>
  void emplace_back(Args&&... args) {
    insert(end(), std::forward<Args>(args)...); 
  }

  template <typename... Args>
  void emplace_front(Args&& ...args) {
    insert(begin(), std::forward<Args>(args)...); 
  }

  void push_front(const T& val) {
    insert(begin(), val);
  }

  void push_front(T&& val) {
    insert(begin(), std::move(val));
  }

  void pop_back() {
    // erase(--end());
    iterator tmp = end();
    erase(--tmp);
  }

  void pop_front() {
    erase(begin()); 
  }

  void Clear() {
    auto iter = begin();
    while (iter != end()) {
      iter = erase(iter);
    }
  }

  iterator insert(const_iterator position, const T& val) {
    Node* tmp = CreateNode(val);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return iterator(tmp);
  }

  iterator insert(const_iterator position, size_type n, const T& val) {
    for (size_type i = 0; i < n; ++i) {
      position = insert(position, val);
    }
    return position;
  }

  template <typename InputIterator>
  iterator insert(const_iterator position, InputIterator first, 
                  InputIterator last) {
    for (; first != last; ++first) {
      position = insert(position, *iter);
    } 
    return position;
  }

  iterator insert(const_iterator position, value_type&& val) {
    Node* tmp = CreateNode(std::move(val));
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }
  
  iterator insert(const_iterator position, std::initializer<value_type> il) {
    return insert(position, il.begin(), il.end());
  }

  //template <typename... Args>
  //iterator insert(const_iterator position, Args&&...  args) {
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

  iterator Begin() { return node->next; }
  const_iterator Begin() const { return node->next; }
  const_iterator Cbegin() const { return node->next; }
  iterator End() { return node; }
  const_iterator End() const { return node; }
  const_iterator Cend() const { return node; }

  bool Empty() const { return node == node->next; }
  size_type Size() const {
    size_type result = std::distance(begin(), end());
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
  void merge(list& x);
  void merge(list&& x);
  template <typename Compare>
  void Merge(list& x, Compare comp);
  template <typename Compare>
  void Merge(List&& x, Compare comp);
  void Sort();
  template <typename Compare>
  void Sort(Compare comp);
  void Reverse() {}

private:
  void InitializeEmpty() {
    node_ = CreateNode();
    node->next = node;
    node->prev = node;
  }

  Node* CreateNode(const value_type& val = value_type()) {
    Node* p = alloc_.allocate(1);
    alloc.construct(&p->data, val); 
    return p;
  }

  Node* CreateNode(value_type&& val) {
    Node* p = alloc_.allocate(1);
    alloc.construct(&p->data, std::move(val)); 
    return p;
  }

  template <typename... Args>
  Node* CreateNode(Args&&... args) {
    Node* p = alloc_.allocate(1);
    alloc.construct(&p->data, std::forward<Args>(args)...);
  }

  void DestroyNode(Node* p) {
    alloc.destroy(p);
    alloc.deallocate(p, 1);
  }

  allocator_type alloc_;
  Node* node_; 
};

} // namespace my

#endif // LIST_H_
