

#ifndef OPERATIONBINARY_H
#define OPERATIONBINARY_H

#include "Operation.h"
#include "Variable.h"
#include "Exceptions.h"
#include "DirectedGraph.h"

#include <memory>

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

  virtual std::unique_ptr<Variable> operator()(const Variable& input1, const Variable& input2) const = 0;

  virtual std::unique_ptr<Variable> operator()(DirectedGraph<Variable*>& graph,
					       Variable& input1, Variable& input2) const = 0;
  

  virtual void uop(const Variable& input, Variable& variable) const override
  {
    throw InvalidOperationException("Unary operation: Variable -> Variable unsupported for binary operation");
  }

  bool isUnary() const override { return false; }

  bool isBinary() const override { return true; }
};

#endif
