
#ifndef OPERATIONS_H
#define OPERATIONS_H


#include "OperationBinary.h"
#include "OperationUnary.h"
#include "Scalar.h"


class ScalarAdd : public OperationBinary
{
  Scalar operator()(const Scalar& input1, const Scalar& input2)
  { return Scalar{*this}; }
  /**
   * @brief Performs the forward operation.
   * @param input1, input2 Input variable node to a binary operation in a computational graph.
   * @param variable Output variable node in a computational graph.
   */
  void op(const Scalar& input1, const Scalar& input2, Scalar& variable)
  { variable.setValue(input1.add(input2)); }
};


class Input : public OperationUnary
{
  void op(const Variable& input, Variable& variable)
  { /* Do nothing since this is the input. */ }
};


#endif

// Recall that virtual functions are functions that resolve into the most derived class.
