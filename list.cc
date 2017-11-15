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
  using link_type = ListNode<T>*;
  using value_type = T;
  using reference = Ref;
  using pointer = Ptr;
  using iterator_category = std::bidirectional_iterator_tag; 
  using difference_type = ptrdiff_t;

  link_type node;
  
  ListIterator() { node = nullptr; }
  ListIterator(link_type x) { node = x; }
  ListIterator(const ListIterator& x) { node = x.node; }

  bool operator==(const ListIterator& x) { return node == x.node; }
  bool operator!=(const ListIterator& x) { return node != x.node; }

  T& operator*() const {
    return node->data;
  }

  T* operator->() const {
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
  using value_type = T;
  using reference = T&;
  using size_type = size_t;
  using iterator = ListIterator<T, T&, T*>; 
  using const_iterator = ListIterator<T, const T&, const T*>; 
private:
  using link_type = ListNode<T>*;

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
    EmptyInitialize();
    for (auto iter = list.Begin(); iter != list.End(); ++iter) {
      Insert(End(), *iter);
    } 
  }

  List& operator=(const List& list) {
  
  }

  List(List&& list) {
    /***
    EmptyInitialize();
    for (auto iter = list.Begin(); iter != list.End(); ++iter) {
      Insert(End(), std::move(*iter));
    } 
    ***/
    node = list.node;
    list.EmptyInitialize();
  }

  List& operator=(List&& list) {
    if (node != list.node) {
      Clear();
      node = list.node;
      list.EmptyInitialize();
    }  
    return *this;
  }

  void PushBack(const T& val) {
    insert(End(), val);
  }
  
  void PushBack(T&& val) {
    Insert(End(), std::move(val));
  }
  
  template <typename... Args>
  void EmplaceBack(Args&& ...args) {
  
  }

  void PushFront(const T& val) {
    Insert(Begin(), val);
  }

  void PushFront(T&& val) {
    Insert(Begin(), std::move(val));
  }

  template <typename... Args>
  void EmplaceFront(Args&& ...args) {
  
  }

  void PopBack() {
    // Erase(--End());
    iterator tmp = End();
    Erase(--tmp);
  }

  void PopFront() {
    Erase(Begin()); 
  }

  void Clear() {
    auto iter = Begin();
    while (iter != End()) {
      iter = Erase(iter);
    }
  }

  iterator Insert(iterator position, const T& val) {
    link_type tmp = CreateNode(val);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }

  iterator Insert(iterator position, T&& val) {
    link_type tmp = CreateNode(val);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
    return tmp;
  }

  iterator Erase(iterator position) {
    link_type next_node = position.node->next;
    link_type prev_node = position.node->prev;
    next_node->prev = prev_node;
    prev_node->next = next_node;
    DestroyNode(position.node);
    return next_node;
  }

  iterator Erase(iterator first, iterator last) {
    auto iter = first;
    while (iter != last) {
      iter = Erase(iter);
    }
    return iter;
  }

  iterator Begin() { return node->next; }
  const_iterator Begin() const { return node->next; }
  iterator End() { return node; }
  const_iterator End() const { return node; }

  bool Empty() const { return node == node->next; }
  size_type Size() const {
    size_type result = std::distance(Begin(), End());
    return result;  
  }

  reference Front() { return *Begin(); }
  reference Back() { return *(--End()); }

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
  my_list.Clear();
  my_list.PushBack(2);  
  my_list.PushBack(3);  
  my_list.PushBack(4);
  my_list.PopBack();

  List<int> list;
  list = std::move(my_list);
  std::cout << list.Size() << std::endl;
  std::cout << my_list.Size() << std::endl;
}


