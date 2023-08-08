
#include "Operation.h"
#include "Scalar.h"

#include <cassert>
#include <memory>
#include <string>

Scalar::Scalar(const std::string& name, const Operation& operation, double value, bool flag)
  : Variable{ name, operation, flag }
{
  m_memory = std::make_unique<double>(value);
}

Scalar::Scalar(const Operation& operation, double value, bool flag)
  : Variable{ operation, flag }
{
  m_memory = std::make_unique<double>(value);
}

double Scalar::add(const Scalar& rscalar) const
{
  return *m_memory + *rscalar.m_memory;
}

double Scalar::subtract(const Scalar& subtrahend) const
{
  return *m_memory - *subtrahend.m_memory;
}

double Scalar::multiply(const Scalar& factor) const
{
  return (*m_memory) * (*factor.m_memory);
}

double Scalar::divide(const Scalar& denominator) const
{
  return *m_memory / *denominator.m_memory;
}

std::ostream& Scalar::print(std::ostream& out) const
{
  auto& name{this->getName()};
  if (name != "")
    {
      out << this->getName() << '=' << *m_memory;
    }
  else
    {
      out << *m_memory;
    }
  return out;
}
