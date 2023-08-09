
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

template <typename F>
bool equals(Unit& f_graph, F f, const double x=2.0, const bool print=false) {
  constexpr double eps{1e-3};
  if (f_graph.forward(x) - f(x) > eps) {
    std::stringstream ss{};
    ss << f_graph.forward(x) << " != " << f(x) << " for x=" << x << '\n';
    throw IncorrectValueException( ss.str() );
  }
  if (print) {
    std::cout << f_graph.forward(x) << " == " << f(x) << " for x=" << x << '\n';
  }
  return true;
}

void testSingularUnit() {
  auto f{[](auto x){ return exp( pow(abs(log(5*x + 3) - 5), 0.5) )/2. ; }};
  // A singular unit is a unit with only one variable term, i.e. e^(log(5*x^5)^2) or (x^3)*5
  // This graph will have no cycles.
  // Unit unit{Scalar{input, 0.0, "x"}};
  // I enjoy the idea of the unit being a central bulding component and that Scalar, Matrix etc
  // are all hidden from the inteface.
  
  Unit fg{ Scalar{"x"} };
  fg.mul(5.0).add(3.0).log().sub(5.0).abs().xpn(0.5).exp().div(2.0);  
  equals(fg, f);
  std::cout << "And then...\n";
}

void testMultiUnit() {
  {
    // x^3 - x^2
    auto f{[](double x) { return x*x*x - x*x; }};
    Unit u{ Scalar{"x", input, 2.0} };
    u.xpn(3).sub(Unit{Scalar{"x", input, 2.0}}.xpn(2));
    u.printGraph();
    equals(u, f, 4.0);
    std::cout << "----------\n";
  }

  {
    // Testing all available operations.
    // ( sqrt(x^3) * log(x) - (x^2 - abs(x)) )/(exp(x) + x)
    auto f{[](double x){
	     return (pow(x*x*x, 0.5) * log(x) - (x*x - abs(x)))/(exp(x) + log(x));
	   }};
    Unit gf{Scalar{"x"}};
    gf
      .xpn(3)
      .xpn(0.5)
      .mul( Unit{Scalar{"x"}}.log() )
      .sub( Unit{Scalar{"x"}}.xpn(2)
	    .sub( Unit{Scalar{"x"}}.abs() ) )
      .div( Unit{Scalar{"x"}}.exp()
	    .add( Unit{Scalar{"x"}}.log() ) );
    
    std::cout << "loop test\n";
    const double arr[] = {0.02133, 0.123, 1.2314, 10.1234};
    for (const double x : arr) {
      equals(gf, f, x);
      std::cout << gf.forward(x) << " ?= " << f(x) << '\n';
    }

    
  }
}

void testJoiningUnits() {
  auto f{ [](double x){ return x/(x+1); } };
  
  Unit fun{Scalar("x")};
  for (int i{0}; i < 3; ++i) {
    // We create multiple layers of g(x) = x/(1 + x)
    Unit gun{Scalar{"x"}};
    gun.div(Unit{Scalar{"x"}}.add(1));
    fun.join(gun);
  }
  const double values[] = {0.1241, 2.123124, 22.123, 123.34};
  for (const double x : values) {
    equals(fun, [&f](double x){ return f(f(f(x))); }, x, true);
  }
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
  testJoiningUnits();
  std::cout << "Test Complete!\n";
}
