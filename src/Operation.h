/**
 * @todo      
 * It would perhaps be appropriate if each variable in the graph has an evaluate function which
 * would use its own operation and inputs to generate a value for itself. We could then walk 
 * through any graph and evaluate each node separately.
 *
 * The idea here is that the operation is separate from the Variable objects.
 * In simple terms, any unary operation is an edge between two Variables in the graph.
 * What the op mthod will do is to modify the current variable's m_value by using the
 * input variable.
 */


// Interface for an operation

#ifndef OPERATION_H
#define OPERATION_H

#include <iostream>

class Operation
{
public:
  
  friend bool operator==(const Operation& op1, const Operation& op2);

  friend std::ostream& operator<<(std::ostream& out, const Operation& op)
  {
    return op.print(out);
  }

  virtual std::ostream& print(std::ostream& out) const
  {
    out << "Operation";
    return out;
  }
};

// May want to move this to other file, although I think it should be fine here as long as
// I mark it as inline.
inline bool operator==(const Operation& op1, const Operation& op2)
{ return &op1 == &op2; }


#endif
