
// Unit test for Scalar.h

#include "Scalar.h"
#include "ScalarAdd.h"
#include "Input.h"
#include "Variable.h"

#include <memory>
#include <cassert>
#include <vector>
#include <iostream>

int main()
{
  const Input input{};

  // We try both a default constructor and the defined one.
  Scalar s_dea{input, 0.0};
  Scalar s_def{input, 1.};


  assert(s_dea.getValue() == 0.0 && "Default init was not 0.0");
  assert(s_def.getValue() == 1.0 && "Defined init set to 1.0 was not 1.o as specified");

  // Note that static asserts can only be called on compile time constants.
  static_assert(s_dea.getDimension() == 0, "Dimension should be equal to 0 for scalars");

  s_def.setValue(2.0);
  assert(s_def.getValue() == 2.0);

  // Test that the operations are working as they should.
  Scalar x{ input, 3.0, "x" };
  Scalar y{ input, 6.0, "y" };
  
  assert( x.add(y) == 9.0 && "Addition not working properly" );
  assert( x.subtract(y) == -3.0 && "Subtraction not working properly");
  assert( x.multiply(y) == 18.0 && "Multiplication not working properly");
  assert( x.divide(y) == 0.5 && "Division not working properly");

  // Test that the flag is working correctly
  x.setTrue();
  y.setFalse();

  assert(x && "x should evaluate to true");
  assert(!y && "y should evaluate to false");

  // Test that the memory handling from Variable.h works correctly.
  auto ptr{x.getMemoryPtr()};
  assert(*ptr == x.getValue() && "Dereferenced raw pointer should point to the same value as function");

  std::unique_ptr<double> new_memory{ std::make_unique<double>(5.0)};
  std::vector<int> new_lengths{};
  x.overwriteMemory(std::move(new_memory), new_lengths);
  assert(x.getValue() == 5.0 && "Memory was not successfully copied into the variable.");

  // Test that the lengths of all these Scalars are all 0.
  assert(x.getLengths().size() == x.getDimension() && y.getLengths().size() == y.getDimension() && "Lengths should be empty since it has dim=0.");

  // Try to print
  std::cout << "Variables for test printed as:\n" << x << '\n' << y << '\n';
  
  // Test that std::move will not move a heap allocated Scalar from its initial position.
  std::unique_ptr<Scalar> z{ std::make_unique<Scalar>(input, 42.0, "z") };
  Variable* old_ptr{ z.get() };

  std::vector<std::unique_ptr<Variable>> vars{};
  vars.push_back(std::move(z));
  
  Variable* new_ptr{vars.at(0).get()};

  assert(!z && "The moved from uptr should be null");
  assert(old_ptr == new_ptr && "With std::move the heap allocation should remain at the same spot in memory.");
  
  // Add more tests here pertaining only to the Scalar object
  return 0;
}
