

#ifndef OPERATIONBINARY_H
#define OPERATIONBINARY_H

#include "Operation.h"
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
  
};

#endif
