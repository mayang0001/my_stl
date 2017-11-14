#include <iostream>

template<typename Iterator, typename T> 
Iterator find(Iterator begin, Iterator end, const T& val) {
  for (Iterator iter = begin; iter != end; ++iter) {
    if (*iter == val) {
      return iter;
    }
  }
  return end;
}

template<typename Iterator, typename Function>
Function for_each(Iterator begin, Iterator end, Function func) {
  for (Iterator iter = begin; iter != end; ++iter) {
    func(*iter);
  }
  return std::move(func);
}

template<typename InputIterator, typename OutputIterator, 
         typename UnaryOperation>
OutputIterator transform(InputIterator begin, InputIterator end,
                         outputIterator result, UnaryOperation op) {

}
