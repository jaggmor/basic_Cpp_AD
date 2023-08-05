
#include "ScalarAdd.h"
#include "checks.h"

#include <cassert>
#include <iostream>


using Gradient = std::vector<double>;


std::unique_ptr<Scalar> ScalarAdd::operator()(const Scalar& input1, const Scalar& input2) const
{
  return std::make_unique<Scalar>(*this, input1.getValue() + input2.getValue());
}

std::unique_ptr<Scalar> ScalarAdd::operator()(DirectedGraph<Variable*>& graph,
					      Scalar& input1, Scalar& input2) const
{
  // Create the resulting scalar on the heap.
  auto res{ std::make_unique<Scalar>(*this, input1.getValue() + input2.getValue()) };

  // Add connections to the graph
  graph.addConnection(&input1, res.get());
  graph.addConnection(&input2, res.get());
    
  return res;
}
  
void ScalarAdd::bop(const Variable& input1, const Variable& input2, Variable& variable) const
{
  assert(isScalar(input1) && isScalar(input2) && isScalar(variable));
  
  double value1{ Scalar::value(input1) };
  double value2{ Scalar::value(input2) };
  *variable.getMemoryPtr() = value1 + value2;
}
  
Gradient ScalarAdd::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  // The gradient of a + b = y w.r.t. a or b is always 1.0.
  return Gradient{ 1.0 * gradient[0] };
}

  
std::ostream& ScalarAdd::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
