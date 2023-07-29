
#ifndef OPERATIONUNARY_H
#define OPERATIONUNARY_H

#include "Operation.h"
#include "Variable.h"
#include "Exceptions.h"
#include "Variable.h"

/**
 * Interface class for a unary operatior. The operator is implemented as a functor that is both
 * able to build the computational graph and also modify the computational graph by calculating
 * forward values and backward gradients.
 * @brief Unary operator base class.
 */
class OperationUnary : public Operation
{  
 public:
  virtual ~OperationUnary() override = default;

  virtual void uop(const Variable& input, Variable& variable) const override
  {
    throw InvalidOperationException("Unsupported operation Variable -> Variable");
  }

  bool isUnary() const override { return true; }
  
  bool isBinary() const override { return false; }
};


#endif
