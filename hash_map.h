#ifndef HASH_MAP_H_
#define HASH_MAP_H_

#include <functional>
#include <vector>
#include <memory>
#include <iterator>

template <typename Value>
struct HashTableNode {
  Value value;
  HashTableNode* next;
};

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
class HashTable;

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
struct HashTableIterator;

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
struct HashTableConstIterator;

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
struct HashTableIterator {
  using Node = HashTableNode<Value>;
  using HashTable = HashTable<Value, Key, HashFcn, ExtractKey, EqualKey>;
  using iterator = HashTableIterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
  using const_iterator = HashTableConstIterator<Value, Key, HashFcn, 
                                                ExtractKey, EqualKey>;

  using iterator_category = std::forward_iterator_tag;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = Value;
  using reference = value_type&;
  using pointer = value_type*;

  HashTableIterator() = default;
  HashTableIterator(Node* node, HashTable* hash_table) 
      : cur(node), ht(hash_table) {}

  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(*this); }
  iterator& operator++() {
    const Node* node = cur;
    cur = cur->next;
    if (cur == nullptr) {
      size_type bucket = ht->BktNum(node->val);
      while (cur == nullptr && ++bucket < ht->buckets_.size()) {
        cur = ht->buckets_[bucket];
      }
    }
    return *this;
  }

  iterator operator++(int) {
    iterator temp = *this;
    ++*this;
    return temp;
  }

  bool operator==(const iterator& iter) const { return cur == iter.cur; }
  bool operator!=(const iterator& iter) const { return cur != iter.cur; }

  Node* cur;
  HashTable* ht;
};

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
struct HashTableConstIterator {
  using Node = HashTableNode<Value>;
  using HashTable = HashTable<Value, Key, HashFcn, ExtractKey, EqualKey>;
  using iterator = HashTableIterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
  using const_iterator = HashTableConstIterator<Value, Key, HashFcn, 
                                                ExtractKey, EqualKey>;

  using iterator_category = std::forward_iterator_tag;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = Value;
  using reference = const value_type&;
  using pointer = const value_type*;

  HashTableConstIterator() = default;
  HashTableConstIterator(const Node* node, const HashTable* hash_table) 
      : cur(node), ht(hash_table) {}

  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(*this); }
  const_iterator& operator++() {
    const Node* node = cur;
    cur = cur->next;
    if (cur == nullptr) {
      size_type bucket = ht->BktNum(node->val);
      while (cur == nullptr && ++bucket < ht->buckets_.size()) {
        cur = ht->buckets_[bucket];
      }
    }
    return *this;
  }

  const_iterator operator++(int) {
    iterator temp = *this;
    ++*this;
    return temp;
  }

  bool operator==(const const_iterator& iter) const { return cur == iter.cur; }
  bool operator!=(const const_iterator& iter) const { return cur != iter.cur; }

  Node* cur;
  HashTable* ht;
};

template <typename Value, typename Key, typename HashFcn, 
          typename ExtractKey, typename EqualKey>
class HashTable {
 public:
  using value_type = Value;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;
  using key_type = Key;
  using hasher = HashFcn;
  using get_key = ExtractKey;
  using equal_key = EqualKey;
  using size_type = size_t;
  using difference_type = ptrdiff_t;

  using iterator = HashTableIterator<Value, Key, HashFcn, 
                                     ExtractKey, EqualKey>;
  using const_iterator = HashTableConstIterator<Value, Key, HashFcn, 
                                                ExtractKey, EqualKey>;

 private:
  using Node = HashTableNode<Value>;

  // TODO
  HashTable() {
    
  }
  
  size_type Size() const { return num_elements_; }
  size_type MaxSize() const { return size_type(-1); }
  bool Empty() const { return Size() == 0; }

  iterator Begin() {
    for (size_type i = 0; i < buckets_.size(); ++i) {
      if (buckets_[i]) {
        return iterator(buckets_[i], this);
      }
    }
    return End();
  }

  const_iterator Begin() const {
    for (size_type i = 0; i < buckets_.size(); ++i) {
      if (buckets_[i]) {
        return const_iterator(buckets_[i], this);
      }
    }
    return End();
  }

  iterator End() {
    return iterator(nullptr, this);
  }

  const_iterator End() const {
    return const_iterator(nullptr, this);
  }

  std::pair<iterator, bool> InsertUnique(const value_type& val) {
    Resize(num_elements_ + 1);
    return InsertUniqueNoResize(val);
  }

  template <typename ForwardIterator>
  void InsertUnique(ForwardIterator first, ForwardIterator last) {
    size_type n = 0;
    std::distance(first, last, n);
    Resize(num_elements_ + n);
    for (; n > 0; --n, ++first) {
      InsertUniqueNoResize(*first);
    }
  }

  iterator Find(const key_type& key) {
    size_type bucket = BktNum(key);
    Node* node;
    for (node = buckets_[bucket]; 
         node && !key_equal(get_key(node->val), key);
         node = node->next) {}
    return iterator(node, this);
  }

  const_iterator Find(const key_type& key) const {
    size_type bucket = BktNum(key);
    const Node* node;
    for (node = buckets_[bucket];
         node && !key_equal(get_key(node->val), key);
         node = node->next) {}
    return const_iterator(node, this);
  }

  size_type Count(const key_type& key) const {
    size_type bucket = BktNum(key);
    Node* node;
    size_type cnt = 0;
    for (node = buckets_[bucket]; node; node = node->next) {
      if (key_equals(get_key(node->val), key)) {
        ++cnt;
      }
    }
    return cnt;
  }

  size_type Erase(const key_type& key) {
    size_type erased = 0;
  }
  void Erase(const iterator& iter) {
  
  }
  void Erase(iterator first, iterator last) {
  
  }

  void Erase(const const_iterator& iter) {
  
  }
  void Erase(const_iterator first, const_iterator last) {
  
  }

  size_type BucketCount() const { return buckets_.size(); }
  size_type MaxBucketCount() const { return prime_list.back(); }
  size_type Bucket(const key_type& key) const { return BktNum(key); }
  size_type BucketSize(size_type bucket) const {
    size_type cnt = 0;
    for (Node* node = buckets_[bucket]; node; node = node->next) {
      ++cnt;
    }
    return cnt;
  }

  float LoadFactor() const { return num_elements_ / BucketCount(); }
  float MaxLoadFactor() const { return max_load_factor_; }
  void MaxLoadFactor(float factor) { max_load_factor_ = factor; }

 private:
  size_type BktNum(const value_type& val) {
    return BktNum(get_key(val), buckets_.size());
  }

  size_type BktNum(const value_type& val, size_type n) {
    return BktNum(get_key(val), n);
  }

  size_type BktNum(const key_type& key, size_type n) const {
    return hash(key) % n;
  }

  void Resize(size_type num_elements);
  
  std::pair<iterator, bool> InsertUniqueNoResize(const value_type& val);

  Node* NewNode(const value_type& val) {
    Node* node = alloc.allocate();
    node->next = nullptr;
    alloc.construct(&node->val, val);
    return node;    
  }

  void DeleteNode(Node* node) {
    alloc.destroy(&node->val);
    alloc.deallocate(node);
  }

  std::vector<Node*> buckets_;
  size_type num_elements_;
  float max_load_factor_;
  std::allocator<Node> alloc;

  long NextPrime(int n) const; 
  static const int num_primes = 28;
  static std::vector<long> prime_list; 
};

template <typename Value, typename Key, typename HF, typename ExK, typename EqK>
std::vector<long> HashTable<Value, Key, HF, ExK, EqK>::prime_list = {
    53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
    196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
    50331653, 100663319, 201326611, 402653189, 805306457, 1610612741,
    3221225473, 4294967291};

template <typename Value, typename Key, typename HF, typename ExK, typename EqK>
long HashTable<Value, Key, HF, ExK, EqK>::NextPrime(int n) const {
  const auto first = prime_list.begin();
  const auto last = prime_list.end();
  const auto pos = std::lower_bound(first, last, n);

  return pos == last ? *(last- 1) : *pos;
}

template <typename Value, typename Key, typename HF, typename ExK, typename EqK>
void HashTable<Value, Key, HF, ExK, EqK>::Resize(size_type num_elements) {
  const size_type old_num_elements = buckets_.size();
  if (num_elements > old_num_elements) {
    const size_type n = NextPrime(old_num_elements);
    if (n > old_num_elements) {
      std::vector<Node*> temp(n, nullptr);

      for (size_type bucket = 0; bucket < buckets_.size(); ++bucket) {
        Node* first = buckets_[bucket];
        while (first) {
          size_type new_bucket = bkt_num(first->val, n);
          buckets_[bucket] = first->next;
          first->next = temp[new_bucket];
          temp[new_bucket]->next = first;
          first = buckets_[bucket];
        }
      }

      buckets_.swap(temp);
    }
  }
}

template <typename Value, typename Key, typename HF, typename ExK, typename EqK>
std::pair<typename HashTable<Value, Key, HF, ExK, EqK>::iterator, bool> 
HashTable<Value, Key, HF, ExK, EqK>
    ::InsertUniqueNoResize(const value_type& val) {
  const size_type bucket = bkt_num(val);

  Node* first = buckets_[bucket];

  for (Node* cur = first; cur; cur = cur->next) {
    if (equal_key(cur->val, first->val)) {
      return std::pair<iterator, bool>(iterator(cur, this), false);
    }
  }

  Node* temp = NewNode(val);
  temp->next = first;
  buckets_[bucket] = temp;
  ++num_elements_;
  return std::pair<iterator, bool>(iterator(temp, this), true);
}

 template <typename K, typename V>
 struct select_first { 
  K operator() (const std::pair<K, V>& value) { return value.first; }
};

template <typename Key, typename T, typename Hash = std::hash<Key>, 
          typename Pred = std::equal_to<Key>>
class HashMap {
 public:
  using HashTable = HashTable<std::pair<Key, T>, Key, Hash, 
                              select_first<Key, T>, Pred>;

  using key_type = typename HashTable::key_type;
  using data_type = T;
  using mapped_type = T;
  using value_type = typename HashTable::value_type;
  using hasher = typename HashTable::hasher;
  using equal_key = typename HashTable::equal_key;
  using size_type = typename HashTable::size_type;
  using difference_type = typename HashTable::difference_type;
  using pointer = typename HashTable::pointer;
  using const_pointer = typename HashTable::const_pointer;
  using iterator = typename HashTable::iterator;
  using const_iterator = typename HashTable::const_iterator;
  using reference = typename HashTable::reference;
  using const_reference = typename HashTable::reference;
 private:
  HashTable hash_table_;

  bool Empty() const { return hash_table_.Empty(); }
  size_type Size() const { return hash_table_.Size(); }
  size_type MaxSize() const { return hash_table_.MaxSize(); }

  iterator Begin() { return hash_table_.Begin(); }
  iterator End() { return hash_table_.End(); }

  const_iterator Begin() const { return hash_table_.Begin(); }
  const_iterator End() const { return hash_table_.End(); }

  data_type& operator[](const key_type& key);

  iterator Find(const key_type& k) { return hash_table_.Find(k); }
  const_iterator Find(const key_type& k) const { return hash_table_.Find(k); }

  // return 0 or 1, since no dupulicates
  size_type Count(const key_type& k) const { return hash_table_.Count(k); }

  template <typename... Args>
  std::pair<iterator, bool> Emplace(Args&&... args);

  std::pair<iterator, bool> Insert(const value_type& val) {
    return hash_table_.Insert(val);
  }
  template <typename P>
  std::pair<iterator, bool> Insert(P&& val);
  template <typename InputIterator>
  void Insert(InputIterator first, InputIterator last) {
    return hash_table_.Insert(first, last);
  }
  void Insert(std::initializer_list<value_type> il) {
    return hash_table_.Insert(il.begin(), il.end());
  }

  iterator Erase(const_iterator pos) {
    return hash_table_.Erase(pos);
  }
  // return the number of elements erased
  size_type Erase(const key_type& key) {
    return hash_table_.Erase(key);
  }
  iterator Erase(const_iterator first, const_iterator last) {
    return hash_table_.Erase(first, last);
  }

  void Clear() { Erase(Begin(), End()); }

  size_type BucketCount() const { return hash_table_.BucketCount(); }
  size_type MaxBucketCount() const { return hash_table_.MaxBucketCount(); }
  size_type Bucket(const key_type& key) const { 
    return hash_table_.Bucket(key); 
  }
  size_type BucketSize(size_type n) const { return hash_table_.BucketSize(n); }

  // size / bucket_count
  float LoadFactor() const { return hash_table_.LoadFactor(); }
  float MaxLoadFactor() const { return hash_table_.MaxLoadFactor(); }
  void MaxLoadFactor(float factor) { hash_table_.MaxLoadFactor(factor); }
};

#endif
