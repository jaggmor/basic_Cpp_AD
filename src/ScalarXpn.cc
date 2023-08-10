
#include "ScalarXpn.h"
#include "checks.h"

#include <cassert>
#include <iostream>
#include <cmath>

using Gradient = std::vector<double>;


std::unique_ptr<Variable> ScalarXpn::operator()(const Variable& base, const Variable& exponent) const
{
  return std::make_unique<Scalar>(*this, std::pow(Scalar::value(base), Scalar::value(exponent)) );
}

std::unique_ptr<Variable> ScalarXpn::operator()(DirectedGraph<Variable*>& graph,
					      Variable& base, Variable& exponent) const
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
  
  *variable.getMemoryPtr() = std::pow(Scalar::value(base), Scalar::value(exponent));
}
  
Gradient ScalarXpn::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			   const Gradient& gradient) const
{
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  if (&diff_var == inputs[0])
    {
      return Gradient{    Scalar::value(*inputs[1])
			* std::pow(Scalar::value(*inputs[0]), Scalar::value(*inputs[1]) - 1.0)
			* gradient[0]
		     };
    }
  else
    {
      return Gradient{    std::log(Scalar::value(*inputs[0]))
			* std::pow(Scalar::value(*inputs[0]), Scalar::value(*inputs[1]) )
			* gradient[0]
		     };
    }
}

  
std::ostream& ScalarXpn::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
