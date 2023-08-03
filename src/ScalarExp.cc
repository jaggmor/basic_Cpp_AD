
#include "ScalarExp.h"
#include "checks.h"
#include <cassert>
#include <cmath>

using Gradient = std::vector<double>;

std::unique_ptr<Scalar> ScalarExp::operator()(const Scalar& input) const
{ return std::make_unique<Scalar>(*this, std::exp( input.getValue() ) ); }

std::unique_ptr<Scalar> ScalarExp::operator()(DirectedGraph<Variable*>& graph, Scalar& input) const
{
  auto res{ ScalarExp::operator()(input) };
  graph.addConnection(&input, res.get());
  return res;
}

void ScalarExp::uop(const Variable& input, Variable& variable) const 
{
  assert(isScalar(input) && isScalar(variable));
    
  *(variable.getMemoryPtr()) = std::exp(value(input));
}

Gradient ScalarExp::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
	       const Gradient& gradient) const 
{
  validateScalarUnaryBprop(inputs, diff_var, gradient);

  return Gradient{ std::exp(value(diff_var)) * gradient[0] };
}

std::ostream& ScalarExp::print(std::ostream& out) const 
{
  out << m_name;
  return out;
}
