
#ifndef OPERATIONUNARY_H
#define OPERATIONUNARY_H

#include "Operation.h"
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

};


#endif
