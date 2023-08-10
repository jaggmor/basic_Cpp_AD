
#include "ScalarDiv.h"
#include "checks.h"

#include <cassert>
#include <iostream>


using Gradient = std::vector<double>;

std::unique_ptr<Variable> ScalarDiv::operator()(const Variable& dividend,
					      const Variable& divisor) const
{
  return std::make_unique<Scalar>(*this, Scalar::value(dividend) / Scalar::value(divisor) );
}

std::unique_ptr<Variable> ScalarDiv::operator()(DirectedGraph<Variable*>& graph, Variable& dividend, Variable& divisor) const
{
  // Create the resulting scalar on the heap.
  auto res{ std::make_unique<Scalar>(*this, Scalar::value(dividend) / Scalar::value(divisor) ) };

  // Add connections to the graph
  graph.addConnection(&dividend, res.get());
  graph.addConnection(&divisor, res.get());

  // We need to ensure that these are inside the inputs as intputs = {dividend, divisor}
  assert(   *res->getInputs(graph).at(0) == dividend
	 && *res->getInputs(graph).at(1) == divisor);
  return res;
}

void ScalarDiv::bop(const Variable& dividend, const Variable& divisor, Variable& variable) const
{

  assert(isScalar(dividend) && isScalar(divisor) && isScalar(variable));
  
  double value1{ *dividend.getMemoryPtr() };
  double value2{ *divisor.getMemoryPtr() };
  *variable.getMemoryPtr() = value1 / value2;
}

Gradient ScalarDiv::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{  
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  // x/y
  double x{ Scalar::value(*inputs[0]) };
  double y{ Scalar::value(*inputs[1]) };
  
  if (inputs.at(0) == &diff_var)
    return Gradient{ 1.0/y * gradient[0] };
  else
    return Gradient{ -x/(y*y) * gradient[0] };
}

std::ostream& ScalarDiv::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
