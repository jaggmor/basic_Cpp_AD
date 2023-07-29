

#ifndef OPERATIONBINARY_H
#define OPERATIONBINARY_H

#include "Operation.h"
#include "Variable.h"
#include "Exceptions.h"
#include "Variable.h"


/**
 * Interface class for a binary operatior. The operator is implemented as a functor that is both
 * able to build the computational graph and also modify the computational graph by calculating
 * forward values and backward gradients.
 * @brief Binary operator base class.
 */
class OperationBinary : public Operation
{
public:
  virtual ~OperationBinary() override = default;

  virtual void bop(const Variable& input1, const Variable& input2, Variable& variable) const override
  {
    throw InvalidOperationException("Unsupported operation: Variable, Variable -> Variable");
  }

  bool isUnary() const override { return false; }

  bool isBinary() const override { return true; }
};

#endif
