#include <memory>
#include <iostream>


template <typename T>
struct ListNode {
  typedef ListNode* pointer;
  pointer next;
  pointer prev;
  T data;
};

template <typename T>
struct ListIterator {
  typedef ListNode<T>* link_type;

  link_type node;
  
  ListIterator() { node = nullptr; }
  ListIterator(link_type x) { node = x; }
  ListIterator(const ListIterator& x) { node = x.node; }

  bool operator==(const ListIterator& x) { return node == x.node; }
  bool operator!=(const ListIterator& x) { return node != x.node; }

  T& operator*() {
    return node->data;
  }

  T* operator->() {
    return &(node->data); 
  }

  link_type operator++() {
    node = node->next;
    return node;
  }

  link_type operator--() {
    node = node->prev;
    return node;
  }
};

template <typename T>
class List {
public:
  typedef T value_type;
  typedef value_type& reference;
  typedef size_t size_type;
  typedef ListIterator<T> iterator;
private:
  typedef ListNode<T>* link_type;

public:
  List() { EmptyInitialize(); }

  explicit List(size_type n) {
  
  }

  List(size_type n, const T& val) {
  
  }

  template <typename Iterator>
  List(Iterator first, Iterator last) {
  
  }

  List(const List& list) {
  
  }

  List(List&& list) {
  
  }

  void PushBack(const T& val) {
    insert(end(), val);
  }
  
  void PushBack(T&& val) {
  
  }
  
  template <typename... Args>
  void EmplaceBack(Args&& ...args) {
  
  }

  void PushFront(const T& val) {
    insert(begin(), val);
  }

  void PushFront(T&& val) {
  
  }

  template <typename... Args>
  void EmplaceFront(Args&& ...args) {
  
  }

  void PopBack() {
    // erase(--end());
    iterator tmp = end();
    erase(--tmp);
  }

  void PopFront() {
    erase(begin()); 
  }

  iterator insert(iterator position, const T& val) {
    link_type tmp = CreateNode(val);
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
    DestroyNode(position.node);
    return next_node;
  }

  iterator begin() { return node->next; }
  iterator end() { return node; }

  bool empty() { return node == node->next; }

  reference front() { return *begin(); }
  reference back() { return *(--end()); }

private:
  link_type GetNode() {
    return alloc.allocate(1);
  }

  void PutNode(link_type p) {
    alloc.deallocate(p, 1);
  }
  
  void EmptyInitialize() {
    node = GetNode();
    node->next = node;
    node->prev = node;
  }

  link_type CreateNode(const T& val) {
    link_type p = GetNode();
    alloc.construct(&p->data, val); 
    return p;
  }

  void DestroyNode(link_type p) {
    alloc.destroy(p);
    PutNode(p);
  }

  std::allocator<ListNode<T>> alloc;
  link_type node; 
};

int main() {
  List<int> my_list;
  my_list.PushBack(1);  
  my_list.PushFront(0);
  my_list.PushBack(2);  
  my_list.PushBack(3);  
  my_list.PushBack(4);

  for (auto iter = my_list.begin(); iter != my_list.end(); ++iter) {
    std::cout << *iter << std::endl;
  }
  my_list.PopBack();
  for (auto iter = my_list.begin(); iter != my_list.end(); ++iter) {
    std::cout << *iter << std::endl;
  }
}


