#include "util.h"
#include <cassert>
#include <iostream>

void test_intersect() {
  // We define a comparator lambda for comparing two ints
  auto comparator{ [](int a, int b) -> bool { return a == b - 10;}};
  // Two vectors to compare
  const std::vector<int> v1{1, 4, 2, 3, 5, 6, 10, 11, 8};
  // NEVER TEST WITH THE SAME VECTOR. HERE WE MISSED A NASTY BUG BECAUSE OF IT.
  const std::vector<int> v2{11, 10, 21, 18, 17};
  
  auto pairs{ util::intersect(v1, v2, comparator) };

  //print and assert
  const int truth[] = {1, 11, 8};
  for (int i{0}; const auto& [e1, e2] : pairs) {
    std::cout << e1 << " - " << e2 << '\n';
    assert(e1 == e2 - 10);
    assert(truth[i++] == e1);
  }
}

int main() {
  test_intersect();
}

