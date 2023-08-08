
#include "Unit.h"
#include "Exceptions.h"
#include "Scalar.h"
#include "operation_constants.h"

#include <iostream>
#include <sstream>
#include <cmath>
#include <typeinfo>

#define VECTOR_TESTS 0
#define FIXED_COPY_CONSTRUCTOR 0

using std::log, std::abs, std::pow, std::exp;
using In = Unit;

void testSingularUnit() {
  auto f{[](auto x){ return exp( pow(abs(log(5*x + 3) - 5), 0.5) )/2. ; }};
  // A singular unit is a unit with only one variable term, i.e. e^(log(5*x^5)^2) or (x^3)*5
  // This graph will have no cycles.
  // Unit unit{Scalar{input, 0.0, "x"}};
  // I enjoy the idea of the unit being a central bulding component and that Scalar, Matrix etc
  // are all hidden from the inteface.
  constexpr double x{1.0};
  Unit fg{ Scalar{"x"} };
  fg.mul(5.0).add(3.0).log().sub(5.0).abs().xpn(0.5).exp().div(2.0);
  if (fg.forward(x) != f(x)) {
    std::stringstream ss{};
    ss << fg.forward(x) << " != " << f(x) << '\n';
    throw IncorrectValueException( ss.str() );
  }
  
  fg.printGraph();
  std::cout << "And then...\n";
  fg.printLeafs();
}

void testMultiUnit() {
  auto f{[](double x) { return x*x*x + x*x; }};
  // x^3 - x^2
  constexpr double x{2.0};
  Unit u{ Scalar{"x", input, 2.0} };
  u.xpn(3).add(Unit{Scalar{"x", input, 2.0}}.xpn(2));
  u.printGraph();
  std::cout << u.forward(x);
  std::cout << " = " << f(x) << '\n';
}



#if FIXED_COPY_CONSTRUCTOR
void testConstructFromRef()
{
  // Turns out that the unique ptrs stored in the vector of Unit causes issues.
  // TODO Implement the rule of three, i.e. copy constructor, copy assignment and destructor.
  Unit u{Scalar{"x", input, 2.0}};
  u.add(2);
  Unit new_u{u};             // Copy will cause crash
  Unit new_u{std::move(u)};  // This will work though

  // In fact any copy of a class that wraps a unique_ptr will cause issues.
  // Hence we may want to create copy constructors for all Variable-s. 
}
#endif


#if 1
void testBetterSynax()
{
  // First we could make it possible to just write Scalar{"x"}.
  Unit u{Scalar{"x"}};
  u.add(5).xpn(2).add(In{Scalar{"x"}}.log().xpn(0.5));
  u.printGraph();
}
#endif

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
  testMultiUnit();
  std::cout << "Test Complete!\n";
}
