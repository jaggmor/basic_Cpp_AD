
#include "Scalar.h"


Scalar::Scalar(double value)
  : m_value{ value }
{
  // Empty constructor
}

Scalar Scalar::add(const Scalar& rscalar) const
{
  return Scalar{ m_value + rscalar.m_value };
}

Scalar Scalar::subtract(const Scalar& subtrahend) const
{
  return Scalar{ m_value - subtrahend.m_value};
}

Scalar Scalar::multiply(const Scalar& factor) const
{
  return Scalar{ m_value * factor.m_value };
}

Scalar Scalar::divide(const Scalar& denominator) const
{
  return Scalar{ m_value / denominator.m_value };
}
