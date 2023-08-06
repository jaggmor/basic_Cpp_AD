
#include "ScalarLog.h"
#include "checks.h"
#include <cassert>
#include <cmath>

using Gradient = std::vector<double>;

std::unique_ptr<Variable> ScalarLog::operator()(const Variable& input) const
{ return std::make_unique<Scalar>(*this, std::log( input.getValue() ) ); }

std::unique_ptr<Variable> ScalarLog::operator()(DirectedGraph<Variable*>& graph,
					      Variable& input) const
{
  auto res{ ScalarLog::operator()(input) };
  graph.addConnection(&input, res.get());
  return res;
}

void ScalarLog::uop(const Variable& input, Variable& variable) const
{
  assert(isScalar(input) && isScalar(variable));
    
  *(variable.getMemoryPtr()) = std::log(Scalar::value(input));
}

Gradient ScalarLog::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{
  validateScalarUnaryBprop(inputs, diff_var, gradient);

  return Gradient{ 1.0/Scalar::value(diff_var) * gradient[0] };
}

std::ostream& ScalarLog::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
