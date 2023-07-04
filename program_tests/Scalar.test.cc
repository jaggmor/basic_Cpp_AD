
// Unit test for Scalar.h

#include "Scalar.h"
#include <cassert>


int main()
{
  // We try both a default constructor and the defined one.
  Scalar s_dea{};
  Scalar s_def{1.};

  // Note that the best practice is to use assertions for cases that should
  // not happen as I have done below

  assert(s_dea.getValue() == 0.0 && "Default init was not 0.0");
  assert(s_def.getValue() == 1.0 && "Defined init set to 1.0 was not 1.o as specified");

  // Note that static asserts can only be called on compile time constants.
  static_assert(s_dea.getDimension() == 0, "Dimension should be equal to 0 for scalars");

  s_def.setValue(2.0);
  assert(s_def.getValue() == 2.0);

  // Add more tests here pertaining only to the Scalar object
  
  return 0;
}
