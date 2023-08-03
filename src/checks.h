
#ifndef CHECKS_H
#define CHECKS_H


#include "Variable.h"
#include <cassert>

using Gradient = std::vector<double>;

inline bool isScalar(const Variable& var) { return (var.getLengths().size() == 0); }

inline bool validateScalarBinaryBprop(const std::vector<Variable*>& inputs,
				      const Variable& diff_var,
				      const Gradient& gradient)
{
  assert(inputs.size() == 2 && "For binary operator we must have 2 inputs only.");
  assert( isScalar(diff_var) );
  for (auto var_ptr : inputs)
    assert( isScalar(*var_ptr) );    
  assert(gradient.size() == 1 && "For scalar operator the gradient for any diff_var is 1D.");
  return true;
}

inline bool validateScalarUnaryBprop(const std::vector<Variable*>& inputs,
				      const Variable& diff_var,
				      const Gradient& gradient)
{
  assert(inputs.size() == 1 && "For unary operator we must have 1 input only.");
  assert( isScalar(diff_var) );
  for (auto var_ptr : inputs)
    assert( isScalar(*var_ptr) );    
  assert(gradient.size() == 1 && "For scalar operator the gradient for any diff_var is 1D.");
  assert(inputs[0] == &diff_var && "Only input should be diff_var");
  return true;
}



#endif
