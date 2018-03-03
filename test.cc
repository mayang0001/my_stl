#include <iostream>
#include "list.h"
#include "hash_map.h"
#include "pair.h"
#include "queue.h"
#include "stack.h"
#include "hash_func.h"

int main() {
  //HashMap<int, int> hash_map;
  //std::cout << hash_map.Size() << std::endl;
  //hash_map.Insert(std::pair<int, int>(10, 20));
  //hash_map.Insert(std::pair<int, int>(20, 30));
  //HashMap<int, int> new_hash_map = hash_map;
  //new_hash_map.Insert(std::pair<int, int>(30, 40));
  //hash_map = new_hash_map;
  //std::cout << hash_map.Erase(10) << std::endl;
  //std::cout << hash_map.Erase(15) << std::endl;
  //hash_map.Erase(hash_map.Begin());
  //for (auto iter = hash_map.Begin(); iter != hash_map.End(); ++iter) {
  //  std::cout << iter->first << " " << iter->second << std::endl;
  //}
  //hash_map.Erase(hash_map.Begin(), hash_map.End());
  //std::cout << hash_map.Size() << std::endl;
  
  //Stack<int> s;
  //s.Push(1);
  //std::cout << s.Top() << std::endl;
  //s.Push(2);
  //s.Pop();
  //s.Push(2);
  //s.Emplace(3);
  //std::cout << s.Size() << std::endl;

  //Queue<int> q;
  //q.Push(1);
  //q.Push(2);
  //std::cout << q.Size() << std::endl;
  //q.Pop();
  //std::cout << q.Front() << std::endl;
  //q.Front() = 3;
  //std::cout << q.Front() << std::endl;

  //std::cout << "Priority Queue" << std::endl;
  //PriorityQueue<int> q;
  //q.Push(3);
  //q.Push(4);
  //q.Push(5);
  //while (!q.Empty()) {
  //  std::cout << q.Top() << std::endl;
  //  q.Pop();
  //}

  //const int a = 1;
  //my::Pair<int, int> p(a, a);
  //std::cout << p.first << " " << p.second << std::endl;

  //p = my::MakePair(1, 2);
  //std::cout << p.first << " " << p.second << std::endl;

  //char c = '5';
  //std::cout << Hash<int>()(c) << std::endl;

  my::List<int> l1;
  l1.PushBack(1);
  l1.PushBack(2);
  l1.PopBack();
  l1.PushFront(0);
  l1.PushFront(-1);
  l1.PopFront();
  l1.Insert(l1.Begin(), 10, 111);
  for (auto iter = l1.Begin(); iter !=  l1.End(); ++iter) {
    *iter *= 2;
    std::cout << *iter << " ";
  }
  std::cout << std::endl;
  std::cout << l1.Size() << std::endl;

  my::List<int> l3(std::move(10));
  std::cout << l3.Size() << std::endl;

  //my::List<int> l3(10, 1);
  //std::cout << l3.Size() << std::endl;

  my::List<int> l4(l3.Begin(), l3.End());
  std::cout << l4.Size() << std::endl;

  my::List<int> l5{1,2,3};
  std::cout << l5.Size() << std::endl;
}
