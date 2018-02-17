#ifndef LIST_H
#define LIST_H

#include <memory>
#include <iostream>
#include <iterator>

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
  using iterator_category = std::bidirectional_iterator_tag; 

  using Node = ListNode<T>;

  Node* node;
  
  ListIterator() { node = nullptr; }
  ListIterator(link_type x) { node = x; }
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

template <typename T>
class List {
public:
  using value_type = T;
  using reference = T&;
  using size_type = size_t;
  using iterator = ListIterator<T, T&, T*>; 
  using const_iterator = ListIterator<T, const T&, const T*>; 
private:
  using link_type = ListNode<T>*;

public:
  List() { empty_initialize(); }

  explicit List(size_type n) {
  
  }

  List(size_type n, const T& val) {
    empty_initialize();
    while (--n) {
      insert(end(), val);
    }
  }

  List(std::initializer_list<T> il) {
    empty_initialize();
    for (auto i : il) {
      insert(end(), i);
    }
  }

  template <typename Iterator>
  List(Iterator first, Iterator last) {
    empty_initialize();
    auto iter = first;
    while (iter != last) {
      insert(end(), *iter);
      ++iter;
    }
  }

  List(const List& list) {
    empty_initialize();
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      insert(end(), *iter);
    } 
  }

  List& operator=(const List& list) {
  
  }

  List(List&& list) {
    /***
    empty_initialize();
    for (auto iter = list.begin(); iter != list.end(); ++iter) {
      insert(end(), std::move(*iter));
    } 
    ***/
    node = list.node;
    list.empty_initialize();
  }

  List& operator=(List&& list) {
    if (node != list.node) {
      Clear();
      node = list.node;
      list.empty_initialize();
    }  
    return *this;
  }

  void push_back(const T& val) {
    insert(end(), val);
  }
  
  void push_back(T&& val) {
    insert(end(), std::move(val));
  }
  
  template <typename... Args>
  void emplace_back(Args&& ...args) {
  
  }

  void push_front(const T& val) {
    insert(begin(), val);
  }

  void push_front(T&& val) {
    insert(begin(), std::move(val));
  }

  template <typename... Args>
  void emplace_front(Args&& ...args) {
  
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

  iterator insert(iterator position, const T& val) {
    link_type tmp = create_node(val);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }

  iterator insert(iterator position, T&& val) {
    link_type tmp = create_node(std::move(val));
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }

  iterator erase(iterator position) {
    link_type next_node = position.node->next;
    link_type prev_node = position.node->prev;
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

  iterator begin() { return node->next; }
  const_iterator begin() const { return node->next; }
  iterator end() { return node; }
  const_iterator end() const { return node; }

  bool empty() const { return node == node->next; }
  size_type Size() const {
    size_type result = std::distance(begin(), end());
    return result;  
  }

  reference front() { return *begin(); }
  reference back() { return *(--end()); }

private:
  link_type get_node() {
    return alloc.allocate(1);
  }

  void put_node(link_type p) {
    alloc.deallocate(p, 1);
  }
  
  void empty_initialize() {
    node = get_node();
    node->next = node;
    node->prev = node;
  }

  link_type create_node(const T& val) {
    link_type p = get_node();
    alloc.construct(&p->data, val); 
    return p;
  }

  link_type create_node(T&& val) {
    link_type p = get_node();
    alloc.construct(&p->data, std::move(val)); 
    return p;
  }

  void destroy_node(link_type p) {
    alloc.destroy(p);
    put_node(p);
  }

  std::allocator<ListNode<T>> alloc;
  link_type node; 
};

#endif
