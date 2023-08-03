
#include "ScalarXpn.h"
#include "checks.h"

#include <cassert>
#include <iostream>
#include <cmath>

using Gradient = std::vector<double>;


std::unique_ptr<Scalar> ScalarXpn::operator()(const Scalar& base, const Scalar& exponent) const
{
  return std::make_unique<Scalar>(*this, std::pow(base.getValue(), exponent.getValue()) );
}

std::unique_ptr<Scalar> ScalarXpn::operator()(DirectedGraph<Variable*>& graph,
					      Scalar& base, Scalar& exponent) const
{
  // Create the resulting scalar on the heap.
  auto res{ ScalarXpn::operator()(base, exponent) };

  // Xpn connections to the graph
  graph.addConnection(&base, res.get());
  graph.addConnection(&exponent, res.get());
    
  return res;
}
  
void ScalarXpn::bop(const Variable& base, const Variable& exponent, Variable& variable) const
{
  assert(isScalar(base) && isScalar(exponent) && isScalar(variable));
  
  *variable.getMemoryPtr() = std::pow(value(base), value(exponent));
}
  
Gradient ScalarXpn::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			   const Gradient& gradient) const
{
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  if (&diff_var == inputs[0])
    {
      return Gradient{    value(*inputs[1])
			* std::pow(value(*inputs[0]), value(*inputs[1]) - 1.0)
			* gradient[0]
		     };
    }
  else
    {
      return Gradient{    std::log(value(*inputs[0]))
			* std::pow(value(*inputs[0]), value(*inputs[1]) )
			* gradient[0]
		     };
    }
}

  
std::ostream& ScalarXpn::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
