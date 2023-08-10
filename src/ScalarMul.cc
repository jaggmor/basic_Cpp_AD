
#include "ScalarMul.h"
#include "checks.h"

#include <cassert>
#include <iostream>


using Gradient = std::vector<double>;


std::unique_ptr<Variable> ScalarMul::operator()(const Variable& input1, const Variable& input2) const
{
  return std::make_unique<Scalar>(*this, Scalar::value(input1) * Scalar::value(input2));
}

std::unique_ptr<Variable> ScalarMul::operator()(DirectedGraph<Variable*>& graph, Variable& input1, Variable& input2) const
{
  // Create the resulting scalar on the heap.
  auto res{ std::make_unique<Scalar>(*this, Scalar::value(input1) * Scalar::value(input2)) };

  // Add connections to the graph
  graph.addConnection(&input1, res.get());
  graph.addConnection(&input2, res.get());
    
  return res;
}
  
void ScalarMul::bop(const Variable& input1, const Variable& input2, Variable& variable) const
{
  assert(isScalar(input1) && isScalar(input2) && isScalar(variable));
  
  double value1{ Scalar::value(input1) };
  double value2{ Scalar::value(input2) };
  *variable.getMemoryPtr() = value1 * value2;
}
  
Gradient ScalarMul::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{
  validateScalarBinaryBprop(inputs, diff_var, gradient);

  // a * b -> b if deriving var is a
  return Gradient{ gradient[0] * ( (inputs.at(0) == &diff_var) ?
				   Scalar::value(*inputs.at(1)) : Scalar::value(*inputs.at(0)) ) }; 
}

  
std::ostream& ScalarMul::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
