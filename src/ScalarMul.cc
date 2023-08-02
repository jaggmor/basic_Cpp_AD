
#include "ScalarMul.h"
#include "checks.h"

#include <cassert>
#include <iostream>


using Gradient = std::vector<double>;


std::unique_ptr<Scalar> ScalarMul::operator()(const Scalar& input1, const Scalar& input2) const
{
  return std::make_unique<Scalar>(*this, input1.getValue() * input2.getValue());
}

std::unique_ptr<Scalar> ScalarMul::operator()(DirectedGraph<Variable*>& graph, Scalar& input1, Scalar& input2) const
{
  // Create the resulting scalar on the heap.
  auto res{ std::make_unique<Scalar>(*this, input1.getValue() * input2.getValue()) };

  // Add connections to the graph
  graph.addConnection(&input1, res.get());
  graph.addConnection(&input2, res.get());
    
  return res;
}
  
void ScalarMul::bop(const Variable& input1, const Variable& input2, Variable& variable) const
{
  assert(isScalar(input1) && isScalar(input2) && isScalar(variable));
  
  double value1{ value(input1) };
  double value2{ value(input2) };
  *variable.getMemoryPtr() = value1 * value2;
}
  
Gradient ScalarMul::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  // a * b -> b if deriving var is a
  return Gradient{ gradient[0] * ( (inputs.at(0) == &diff_var) ?
				   value(*inputs.at(1)) : value(*inputs.at(0)) ) }; 
}

  
std::ostream& ScalarMul::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
