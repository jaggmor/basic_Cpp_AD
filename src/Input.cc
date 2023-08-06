
#include "Input.h"
#include "Exceptions.h"

std::unique_ptr<Variable> Input::operator()(const Variable& input) const
{
  throw InvalidOperationException("Input does not have operator() capabilities.");
}

std::unique_ptr<Variable> Input::operator()(DirectedGraph<Variable*>& graph, Variable& input) const
{
  throw InvalidOperationException("Input does not have operatior() capabilities.");
}

void Input::uop(const Variable& input, Variable& variable) const
{
  throw InvalidOperationException("Input does not have uop capabilities.");
}

Gradient Input::bprop(const std::vector<Variable*>& inputs,
		      const Variable& diff_var, const Gradient& gradient) const
{
  throw InvalidOperationException("Input does not have bprop capabilities.");  
}

std::ostream& Input::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
