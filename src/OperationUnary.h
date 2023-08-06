
#ifndef OPERATIONUNARY_H
#define OPERATIONUNARY_H

#include "Operation.h"
#include "Variable.h"
#include "Exceptions.h"
#include "DirectedGraph.h"

#include <memory>
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

  virtual std::unique_ptr<Variable> operator()(const Variable& input) const = 0;

  virtual std::unique_ptr<Variable> operator()(DirectedGraph<Variable*>& graph, Variable& input) const = 0;
  
  void bop(const Variable& input1, const Variable& input2, Variable& variable) const override
  {
    throw InvalidOperationException("Binary operation Variable, Variable -> Variable is unsupported for unary operation.");
  }

  bool isUnary() const override { return true; }
  
  bool isBinary() const override { return false; }
};


#endif
