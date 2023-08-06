
#include "Unit.h"
#include "Exceptions.h"
#include "Scalar.h"
#include "operation_constants.h"

#include <iostream>
#include <sstream>
#include <cmath>

#define VECTOR_TESTS 0

using std::log, std::abs, std::pow, std::exp;

void testSingularUnit() {
  auto f{[](auto x){ return exp( pow(abs(log(5*x + 3) - 5), 0.5) )/2. ; }};
  // A singular unit is a unit with only one variable term, i.e. e^(log(5*x^5)^2) or (x^3)*5
  // This graph will have no cycles.
  // Unit unit{Scalar{input, 0.0, "x"}};
  // I enjoy the idea of the unit being a central bulding component and that Scalar, Matrix etc
  // are all hidden from the inteface.
  constexpr double x{1.0};
  Unit fg{ Scalar{input, 0.0, "x"} };
  fg.mul(5.0).add(3.0).log().sub(5.0).abs().xpn(0.5).exp().div(2.0);
  if (fg.forward(x) != f(x)) {
    std::stringstream ss{};
    ss << fg.forward(x) << " != " << f(x) << '\n';
    throw IncorrectValueException( ss.str() );
  }
}

#if VECTOR_TESTS
// This is not implemented yet
void testVectorUnitCreation() {
  Unit unit{Vector("x", 100)};
  // Let's pretend we are making a NN layer.
  unit.matmul(W).add(b).relu().connect(W, otherUnit).connect(b, otherUnit);
}

#endif

int main() {
  std::cout << "Test start\n";
  testSingularUnit();
  std::cout << "Test Complete!\n";
}
