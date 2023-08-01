
#include "ScalarSub.h"
#include "checks.h"

#include <cassert>
#include <iostream>


using Gradient = std::vector<double>;

std::unique_ptr<Scalar> ScalarSub::operator()(const Scalar& minuend,
					      const Scalar& subtrahend) const
{
  return std::make_unique<Scalar>(*this, minuend.getValue() - subtrahend.getValue());
}

std::unique_ptr<Scalar> ScalarSub::operator()(Scalar& minuend, Scalar& subtrahend,
				   DirectedGraph<Variable*>& graph) const
{
  // Create the resulting scalar on the heap.
  auto res{ std::make_unique<Scalar>(*this, minuend.getValue() - subtrahend.getValue()) };

  // Add connections to the graph
  graph.addConnection(&minuend, res.get());
  graph.addConnection(&subtrahend, res.get());

  // We need to ensure that these are inside the inputs as intputs = {minuend, subtrahend}
  assert(   *res->getInputs(graph).at(0) == minuend
	 && *res->getInputs(graph).at(1) == subtrahend);
  return res;
}

void ScalarSub::bop(const Variable& minuend, const Variable& subtrahend, Variable& variable) const
{

  assert(isScalar(minuend) && isScalar(subtrahend) && isScalar(variable));
  
  double value1{ *minuend.getMemoryPtr() };
  double value2{ *subtrahend.getMemoryPtr() };
  *variable.getMemoryPtr() = value1 - value2;
}

Gradient ScalarSub::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{  
  validateScalarBinaryBprop(inputs, diff_var, gradient);
  // The gradient of a + b = y w.r.t. a or b is always 1.0.
  if (inputs.at(0) == &diff_var)
    return Gradient{ 1.0 * gradient[0] };
  else
    return Gradient{ -1.0 * gradient[0] };
}

std::ostream& ScalarSub::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
