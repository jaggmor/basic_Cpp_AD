
#include "Operation.h"
#include "Scalar.h"

Scalar::Scalar(const Operation& operation, double value)
  : Variable{ operation }
  , m_value{ value }
{
  // Empty constructor
}

double Scalar::add(const Scalar& rscalar) const
{
  return m_value + rscalar.m_value;
}

double Scalar::subtract(const Scalar& subtrahend) const
{
  return m_value - subtrahend.m_value;
}

double Scalar::multiply(const Scalar& factor) const
{
  return m_value * factor.m_value;
}

double Scalar::divide(const Scalar& denominator) const
{
  return m_value / denominator.m_value;
}

std::ostream& Scalar::print(std::ostream& out) const
{
  out << "Scalar<" << this->getOperation() << '>' << '=' << m_value;
  return out;
}
