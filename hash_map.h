#ifndef HASH_MAP_H_
#define HASH_MAP_H_

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
  using iterator = HashTableIterator<Value, Key, HashFcn, ExtractKey, EquakKey>;
  using const_iterator = HashTableConstIterator<Value, Key, HashFcn, 
                                                ExtractKey, EquakKey>;

  using iterator_category = forward_iterator_tag;
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
    const Node* n = cur;
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
  using iterator = HashTableIterator<Value, Key, HashFcn, ExtractKey, EquakKey>;
  using const_iterator = HashTableConstIterator<Value, Key, HashFcn, 
                                                ExtractKey, EquakKey>;

  using iterator_category = forward_iterator_tag;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = Value;
  using reference = const value_type&;
  using pointer = const value_type*;

  HashTableIterator() = default;
  HashTableIterator(const Node* node, const HashTable* hash_table) 
      : cur(node), ht(hash_table) {}

  reference operator*() const { return cur->val; }
  pointer operator->() const { return &(*this); }
  const_iterator& operator++() {
    const Node* n = cur;
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
  using key_type = Key;
  using hash = HashFcn;
  using get_key = ExtractKey;
  using equal_key = EqualKey;
  using size_type = size_t;

  // TODO
  HashTable() {
    
  }
  
  size_type Size() { return num_elements_; }
  size_type Size() { return size_type(-1); }
  bool Empty() { return Size() == 0; }

  iterator Begin() {
    for (size_type i = 0; i < buckets_.size(); ++i) {
      if (buckets_[i]) {
        return iterator(buckets_[i], this);
      }
    }
    return End();
  }

  iterator End() {
    return iterator(nullptr, this);
  }

  const_iterator Begin() const {
    for (size_type i = 0; i < buckets_.size(); ++i) {
      if (buckets_[i]) {
        return iterator(buckets_[i], this);
      }
    }
    return End();
  }

  const_iterator End() const {
    return const_iterator(nullptr, this);
  }

  std::pair<iterator, bool> InsertUnique(const value_type& val) {
    Resize(num_elements_ + 1);
    return InsertUniqueNoResize(val);
  }

  size_type BucketCount() const { return buckets_.size(); }
  size_type MaxBucketCount() const { return prime_list.back(); }

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

  using Node = HashTableNode<Value>;
  std::allocator<Node> alloc;
  std::vector<Node*> buckets_;
  size_type num_elements_;

  int NextPrime(int n); 
  static const int num_primes = 28;
  static const std::vector<int> prime_list = 
      {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
       196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
       50331653, 100663319, 201326611, 402653189, 805306457, 1610612741,
       3221225473, 4294967291};
};

template <typename Value, typename Key, typename HashFcn, 
          typename ExtractKey, typename EqualKey>
int HashTable::NextPrime(int n) {
  const auto first = prime_list.begin();
  const auto last = prime_list.end();
  const auto pos = std::lower_bound(first, last, n);

  return pos == last ? *(last- 1) : *pos;
}

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
void HashTable::Resize(size_type num_elements) {
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

template <typename Value, typename Key, typename HashFcn,
          typename ExtractKey, typename EqualKey>
std::pair<iterator, bool> InsertUniqueNoResize(const value_type& val) {
  const size_type bucket = bkt_num(val);

  Node* first = buckets_[bucket];

  for (Node* cur = first; cur; cur = cur->next) {
    if (equal_key(cur->val, first->val)) {
      return pair<iterator, bool>(iterator(cur, this), false);
    }
  }

  Node* temp = NewNode(val);
  temp->next = first;
  buckets_[bucket] = temp;
  ++num_elements_;
  return std::pair<iterator, bool>(iterator(temp, this), true);
}

template <typename Key, typename T, typename Hash = hash<Key>, 
          typename Pred = equal_to<Key>>
class HashMap {
 public:
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<Key, T>;
  using hasher = Hash;
  using key_equal = Pred;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = HashTableIterator<value_type, key_type, hash, /*TODO*/, key_equal>;
  using const_iterator = iterator;
  using size_type = size_t;

  bool Empty() const;
  size_type Size() const;
  size_type MaxSize() const;

  iterator Begin();
  iterator End();

  const_iterator Begin() const;
  const_iterator End() const;

  key_type& operator()[];

  iterator Find(const key_type& k);
  const_iterator Find(const key_type& k) const;

  // return 0 or 1, since no dupulicates
  size_type Count(const key_type& k) const;

  template <typename... Args>
  std::pair<iterator, bool> Emplace(Args&&... args);

  std::pair<iterator, bool> Insert(const value_type& val);
  template <typename P>
  std::pair<iterator, bool> Insert(P&& val);
  template <typename InputIterator>
  void Insert(InputIterator first, InputIterator last);
  void Insert(std::initializer_list<value_type> il);

  iterator Erase(const_iterator pos);
  // return the number of elements erased
  size_type Erase(const key_type& key);
  iterator Erase(const_iterator first, const_iterator last);

  void Clear();

  size_type BucketCount() const;
  size_type MaxBucketCount() const;
  size_type Bucket(const key_type& key) const;
  size_type BucketSize(size_type n) const;

  // size / bucket_count
  float LoadFactor() const;
  float MaxLoadFactor() const;
  void MaxLoadFactor(float factor);
};

#endif
