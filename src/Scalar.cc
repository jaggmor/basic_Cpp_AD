
#include "Operation.h"
#include "Scalar.h"
#include <cassert>
#include <memory>

constexpr int gscalar_length{ 1 };

Scalar::Scalar(const Operation& operation, double value, bool flag)
  : Variable{ operation, flag }
{
  // Length of a scalar will always be 1
  m_memory = std::make_unique<double>(value);
  // The lengths list is empty for scalars since we know that is must have only one element and no dimensions!
  // m_lengths.push_back(gscalar_length);
  // assert(m_lengths.size() == 1 && "The vector should always have one element only for a scalar.");
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
  out << "Scalar<" << this->getOperation() << '>' << '=' << *m_memory;
  return out;
}
