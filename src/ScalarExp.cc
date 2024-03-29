
#include "ScalarExp.h"
#include "checks.h"
#include <cassert>
#include <cmath>

using Gradient = std::vector<double>;

std::unique_ptr<Variable> ScalarExp::operator()(const Variable& input) const
{ return std::make_unique<Scalar>(*this, std::exp( Scalar::value(input) ) ); }

std::unique_ptr<Variable> ScalarExp::operator()(DirectedGraph<Variable*>& graph, Variable& input) const
{
  auto res{ ScalarExp::operator()(input) };
  graph.addConnection(&input, res.get());
  return res;
}

void ScalarExp::uop(const Variable& input, Variable& variable) const 
{
  assert(isScalar(input) && isScalar(variable));
    
  *(variable.getMemoryPtr()) = std::exp(Scalar::value(input));
}

Gradient ScalarExp::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
	       const Gradient& gradient) const 
{
  validateScalarUnaryBprop(inputs, diff_var, gradient);

  return Gradient{ std::exp(Scalar::value(diff_var)) * gradient[0] };
}

std::ostream& ScalarExp::print(std::ostream& out) const 
{
  out << m_name;
  return out;
}
