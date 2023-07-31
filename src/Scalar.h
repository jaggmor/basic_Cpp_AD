
// Scalar class for scalars used with the operations to create coumputational graphs.

#ifndef SCALAR_H
#define SCALAR_H

#include "Operation.h"
#include "Variable.h"

#include <memory>
#include <string>

class Scalar : public Variable
{
private:
  static constexpr int m_dimension{ 0 };
  
public:
  Scalar(const Operation& operation, double value=0.0,
	 const std::string& name="", bool flag=false);
  
  void setValue(double value) { *m_memory = value; };
  double getValue() const { return *m_memory; };
  
  constexpr int getDimension() const { return m_dimension; };
  
  double add(const Scalar& rscalar) const;
  double subtract(const Scalar& subtrahend) const;
  double multiply(const Scalar& factor) const;
  double divide(const Scalar& denominator) const;

  std::ostream& print(std::ostream& out) const override;
};

#endif

