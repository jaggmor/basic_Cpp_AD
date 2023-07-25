
// Scalar class for scalars used with the operations to create coumputational graphs.

#ifndef SCALAR_H
#define SCALAR_H

#include "Variable.h"
#include "Operation.h"


class Scalar : public Variable
{
private:
  double m_value {};
  static constexpr int m_dimension{ 0 };
  
public:
  Scalar(const Operation& operation, double value=0.0);
  
  void setValue(double value) { m_value = value; };
  double getValue() const { return m_value; };
  
  constexpr int getDimension() const { return m_dimension; };
  
  double add(const Scalar& rscalar) const;
  double subtract(const Scalar& subtrahend) const;
  double multiply(const Scalar& factor) const;
  double divide(const Scalar& denominator) const;

  std::ostream& print(std::ostream& out) const override;
};

#endif

