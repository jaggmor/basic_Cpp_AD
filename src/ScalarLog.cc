
#include "ScalarLog.h"
#include "checks.h"
#include <cassert>
#include <cmath>

using Gradient = std::vector<double>;

std::unique_ptr<Scalar> ScalarLog::operator()(const Scalar& input) const
{ return std::make_unique<Scalar>(*this, std::log( input.getValue() ) ); }

std::unique_ptr<Scalar> ScalarLog::operator()(DirectedGraph<Variable*>& graph,
					      Scalar& input) const
{
  auto res{ ScalarLog::operator()(input) };
  graph.addConnection(&input, res.get());
  return res;
}

void ScalarLog::uop(const Variable& input, Variable& variable) const
{
  assert(isScalar(input) && isScalar(variable));
    
  *(variable.getMemoryPtr()) = std::log(value(input));
}

Gradient ScalarLog::bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
			  const Gradient& gradient) const
{
  validateScalarUnaryBprop(inputs, diff_var, gradient);

  return Gradient{ 1.0/value(diff_var) * gradient[0] };
}

std::ostream& ScalarLog::print(std::ostream& out) const
{
  out << m_name;
  return out;
}