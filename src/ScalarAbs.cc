
#include "ScalarAbs.h"
#include "checks.h"
#include <cassert>
#include <cmath>

using Gradient = std::vector<double>;

std::unique_ptr<Scalar> ScalarAbs::operator()(const Scalar& input) const
{ return std::make_unique<Scalar>(*this, std::abs( input.getValue() ) ); }

std::unique_ptr<Scalar> ScalarAbs::operator()(DirectedGraph<Variable*>& graph, Scalar& input) const
{
  auto res{ ScalarAbs::operator()(input) };
  graph.addConnection(&input, res.get());
  return res;
}

void ScalarAbs::uop(const Variable& input, Variable& variable) const 
{
  assert(isScalar(input) && isScalar(variable));
    
  *(variable.getMemoryPtr()) = std::abs(Scalar::value(input));
}

Gradient ScalarAbs::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
	       const Gradient& gradient) const 
{
  validateScalarUnaryBprop(inputs, diff_var, gradient);

  return Gradient{ ( (Scalar::value(diff_var) >= 0.0) ? 1. : -1. ) * gradient[0] };
}

std::ostream& ScalarAbs::print(std::ostream& out) const 
{
  out << m_name;
  return out;
}
