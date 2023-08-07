
#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <utility>

namespace util
{
  /**
   * @brief A function that returns the ordered intersection of two lists as a vector of pairs
   * @note O(n^2)
   */
  template <typename T, typename U>
  std::vector<std::pair<T, T>> intersect(const std::vector<T>& vecFirst,
					 const std::vector<T>& vecSecond,
					 U comparator) {
    std::vector<std::pair<T, T>> pairs{};
    for (T elf : vecFirst) {
      // pretty funny bug happened here. Not found with unit testing. vecFirs->vecSecond
      auto it{ std::find_if(vecSecond.begin(), vecSecond.end(),
			    [&comparator, elf](T el) -> bool { return comparator(elf, el); })
      };
      if (it != vecSecond.end()) {
	pairs.push_back( std::make_pair(elf, *it) );
      }
    }
    return pairs;
  }

}
#endif
