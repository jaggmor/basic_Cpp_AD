
// Scalar class for scalars used with the operations to create coumputational graphs.

#ifndef SCALAR_H
#define SCALAR_H

#include "Operation.h"
#include "Variable.h"
#include "input_constant.h"

#include <memory>
#include <string>

class Scalar : public Variable
{
private:
  static constexpr int m_dimension{ 0 };
  
public:
  Scalar(const std::string& name, const Operation& operation=input, // Specific name => Input
	 double value=0.0, bool flag=false);
  Scalar(const Operation& operation, double value=0.0, bool flag=false);
  
  constexpr int getDimension() const { return m_dimension; };

  // Kind of depreciated...
  double add(const Scalar& rscalar) const;
  double subtract(const Scalar& subtrahend) const;
  double multiply(const Scalar& factor) const;
  double divide(const Scalar& denominator) const;

  std::ostream& print(std::ostream& out) const override;

  static inline double value(const Variable& var)
  { return *var.getMemoryPtr(); }

  static inline void setValue(Variable& var, double value)
  { *var.getMemoryPtr() = value; }
};

#endif

