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

  reference operator*() const { return cur->value; }
  pointer operator->() const { return &(cur->value); }
  iterator& operator++() {
    const Node* node = cur;
    cur = cur->next;
    if (cur == nullptr) {
      size_type bucket = ht->BktNum(node->value);
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
 public:
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

  reference operator*() const { return cur->value; }
  pointer operator->() const { return &(cur->value); }
  const_iterator& operator++() {
    const Node* node = cur;
    cur = cur->next;
    if (cur == nullptr) {
      size_type bucket = ht->BktNum(node->value);
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

  friend struct HashTableIterator<Value, Key, HashFcn, ExtractKey, EqualKey>;
  friend struct HashTableConstIterator<Value, Key, HashFcn, 
                                       ExtractKey, EqualKey>;

 private:
  using Node = HashTableNode<Value>;

 public:
  // n for capacity
  HashTable(size_type n, const HashFcn& hf, const ExtractKey& exk, 
            const EqualKey& eqk) 
      : num_elements_(0),
        hash_fcn_(hf),
        extract_key_(exk),
        equal_key_(eqk) {
    const size_type bucket_size = NextSize(n);
    buckets_.reserve(bucket_size);
    buckets_.insert(buckets_.end(), bucket_size, nullptr);
  }
  
  HashTable(const HashTable& ht) 
      : num_elements_(ht.num_elements_),
        hash_fcn_(ht.hash_fcn_),
        extract_key_(ht.extract_key_),
        equal_key_(ht.equal_key_) {
    CopyFrom(ht);
  }

  HashTable& operator=(const HashTable& ht) {
    if (this != &ht) {
      Clear();
      num_elements_ = ht.num_elements_;
      hash_fcn_ = ht.hash_fcn_;
      extract_key_ = ht.extract_key_;
      equal_key_ = ht.equal_key_;
      CopyFrom(ht);
    } 
    return *this;
  }

  ~HashTable() { Clear(); }

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

  std::pair<iterator, bool> Insert(const value_type& val) {
    Resize(num_elements_ + 1);
    return InsertUniqueNoResize(val);
  }

  template <typename ForwardIterator>
  void Insert(ForwardIterator first, ForwardIterator last) {
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
    const size_type bucket = BktNum(key);
    Node* first = buckets_[bucket];
    if (first == nullptr) return 0;

    size_type erased = 0;
    Node* cur = first;
    Node* next = cur->next;
    while (next) {
      if (equal_key_(extract_key_(next->value), key)) {
        cur->next = next->next;
        DeleteNode(next);
        next = cur->next;
        ++erased;
        --num_elements_;
      } else {
        cur = next;
        next = next->next;  
      }
    }
    if (equal_key_(extract_key_(first->value), key)) {
      buckets_[bucket] = first->next;
      DeleteNode(first);
      ++erased;
      --num_elements_;
    }
    return erased;
  }

  iterator Erase(const iterator& iter) {
    Node* p = iter.cur;
    if (p == nullptr) iter; 

    const size_type bucket = BktNum(p->value);
    Node* cur = buckets_[bucket];
    iterator ret_iter = iter;
    if (cur == p) {
      buckets_[bucket] = cur->next;
      DeleteNode(cur);
      --num_elements_;  
      return ++ret_iter;
    } else {
      Node* next = cur->next;
      while (next) {
        if (next == p) {
          cur->next = next->next;
          DeleteNode(next);
          --num_elements_;
          return ++ret_iter;
        }
        cur = next;
        next = next->next;
      }
    }
    return ret_iter;
  }

  iterator Erase(const iterator& first, const iterator& last) {
    if (first == last) return last;

    size_type f_bucket = first.cur ? BktNum(first.cur->value) : buckets_.size();
    size_type l_bucket = last.cur ? BktNum(last.cur->value) : buckets_.size();

    if (f_bucket == l_bucket) {
      EraseBucket(f_bucket, first.cur, last.cur);
    } else {
      EraseBucket(f_bucket, first.cur, nullptr);
      for (size_type bucket = f_bucket + 1; bucket < l_bucket; ++bucket) {
        EraseBucket(bucket, buckets_[bucket], nullptr);
      }
      if (l_bucket != buckets_.size()) {
        EraseBucket(l_bucket, buckets_[l_bucket], last.cur);
      }
    }
    return last;
  }

  iterator Erase(const const_iterator& iter) {
    return Erase(iterator(const_cast<Node*>(iter->cur),
                          const_cast<HashTable*>(iter->ht)));
  }

  iterator Erase(const_iterator first, const_iterator last) {
    return Erase(iterator(const_cast<Node*>(first->cur),
                          const_cast<HashTable>(first->ht)),
                 iterator(const_cast<Node*>(last->cur),
                          const_cast<HashTable*>(last->ht))); 
  }

  void Clear() {
    for (size_type bucket; bucket < buckets_.size(); ++bucket) {
      Node* cur = buckets_[bucket];
      while (cur) {
        Node* next = cur->next;
        DeleteNode(cur);
        cur = next;
      }
      buckets_[bucket] = nullptr;
    }
    num_elements_ = 0;
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
  size_type BktNum(const value_type& val) const {
    return BktNum(extract_key_(val), buckets_.size());
  }

  size_type BktNum(const value_type& val, size_type n) const {
    return BktNum(extract_key_(val), n);
  }

  size_type BktNum(const key_type& key) const {
    return BktNum(key, buckets_.size());
  }

  size_type BktNum(const key_type& key, size_type n) const {
    return hash_fcn_(key) % n;
  }

  void EraseBucket(size_type bucket, Node* first, Node* last) {
    if (buckets_[bucket] == first) {
      Node* cur = buckets_[bucket];
      while (cur != last) {
        DeleteNode(cur);
        --num_elements_;
        cur = cur->next;
      }
      buckets_[bucket] = last;
    } else {
      Node* cur = buckets_[bucket];
      Node* next = cur->next;
      while (next && next != first) {
        cur = next;
        next = cur->next;
      }
      while (next != last) {
        DeleteNode(next);
        next = next->next;
        --num_elements_;
      }
      cur->next = last;
    }
  }

  void Resize(size_type num_elements);
  
  std::pair<iterator, bool> InsertUniqueNoResize(const value_type& val);

  Node* NewNode(const value_type& val) {
    Node* node = alloc.allocate(1);
    node->next = nullptr;
    alloc.construct(&node->value, val);
    return node;    
  }

  void DeleteNode(Node* node) {
    alloc.destroy(node);
    alloc.deallocate(node, sizeof(Node));
  }

  void CopyFrom(const HashTable& hash_table);

  std::vector<Node*> buckets_;
  size_type num_elements_;
  hasher hash_fcn_;
  get_key extract_key_;
  equal_key equal_key_;

  float max_load_factor_;
  std::allocator<Node> alloc;

  unsigned long NextSize(unsigned long n) const; 
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
unsigned long 
HashTable<Value, Key, HF, ExK, EqK>::NextSize(unsigned long n) const {
  const auto first = prime_list.begin();
  const auto last = prime_list.end();
  const auto pos = std::lower_bound(first, last, n);

  return pos == last ? *(last- 1) : *pos;
}

template <typename Value, typename Key, typename HF, typename ExK, typename EqK>
void HashTable<Value, Key, HF, ExK, EqK>::Resize(size_type num_elements) {
  const size_type old_num_elements = buckets_.size();
  if (num_elements > old_num_elements) {
    const size_type n = NextSize(num_elements);
    if (n > old_num_elements) {
      std::vector<Node*> temp(n, nullptr);

      for (size_type bucket = 0; bucket < buckets_.size(); ++bucket) {
        Node* first = buckets_[bucket];
        while (first) {
          size_type new_bucket = BktNum(first->value, n);
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
  const size_type bucket = BktNum(val);

  Node* first = buckets_[bucket];

  for (Node* cur = first; cur; cur = cur->next) {
    if (equal_key_(extract_key_(cur->value), extract_key_(val))) {
      return std::pair<iterator, bool>(iterator(cur, this), false);
    }
  }

  Node* temp = NewNode(val);
  temp->next = first;
  buckets_[bucket] = temp;
  ++num_elements_;
  return std::pair<iterator, bool>(iterator(temp, this), true);
}

template <typename V, typename K, typename HF, typename ExK, typename EqK>
void HashTable<V, K, HF, ExK, EqK>::CopyFrom(const HashTable& hash_table) {
  // TODO Why
  buckets_.clear();
  buckets_.reserve(hash_table.buckets_.size());
  buckets_.insert(buckets_.end(), hash_table.buckets_.size(), nullptr);

  for (size_type bucket = 0; bucket < hash_table.buckets_.size(); ++bucket) {
    if (Node* cur = hash_table.buckets_[bucket]) {
      Node* copy = NewNode(cur->value);
      buckets_[bucket] = copy;

      for (Node* next = cur->next; next; cur = next, next = cur->next) {
        copy->next = NewNode(next->value);
        copy = copy->next; 
      }
    }
  }
  num_elements_ = hash_table.num_elements_;
}

template <class _Pair>
struct select_first : public std::unary_function<_Pair, typename _Pair::first_type> {
  const typename _Pair::first_type& operator()(const _Pair& __x) const {
    return __x.first;
  }
};

template <typename Key, typename T, typename Hash = std::hash<Key>, 
          typename Pred = std::equal_to<Key>>
class HashMap {
 private:
  using HashTable = HashTable<std::pair<Key, T>, Key, Hash, 
                              select_first<std::pair<Key, T>>, Pred>;
  HashTable hash_table_;

 public:
  using key_type = typename HashTable::key_type;
  using data_type = T;
  using mapped_type = T;
  using value_type = typename HashTable::value_type;
  using hasher = typename HashTable::hasher;
  using extract_key = typename HashTable::get_key;
  using equal_key = typename HashTable::equal_key;
  using size_type = typename HashTable::size_type;
  using difference_type = typename HashTable::difference_type;
  using pointer = typename HashTable::pointer;
  using const_pointer = typename HashTable::const_pointer;
  using iterator = typename HashTable::iterator;
  using const_iterator = typename HashTable::const_iterator;
  using reference = typename HashTable::reference;
  using const_reference = typename HashTable::reference;

  HashMap() : hash_table_(100, hasher(), extract_key(), equal_key()) {}
  HashMap(const HashMap& hm) = default;
  HashMap& operator=(const HashMap& hm) = default;
  HashMap(HashMap&& hm) = default;
  HashMap& operator=(HashMap&& hm) = default;
  ~HashMap() = default;

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
  //template <typename P>
  //std::pair<iterator, bool> Insert(P&& val);
  template <typename InputIterator>
  void Insert(InputIterator first, InputIterator last) {
    return hash_table_.Insert(first, last);
  }
  void Insert(std::initializer_list<value_type> il) {
    return hash_table_.Insert(il.begin(), il.end());
  }

  // return the number of elements erased
  size_type Erase(const key_type& key) {
    return hash_table_.Erase(key);
  }
  iterator Erase(iterator pos) {
    return hash_table_.Erase(pos);
  }
  iterator Erase(const_iterator pos) {
    return hash_table_.Erase(pos);
  }
  iterator Erase(iterator first, iterator last) {
    return hash_table_.Erase(first, last);
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
