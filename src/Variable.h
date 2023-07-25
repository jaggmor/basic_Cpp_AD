
// An inheritable class for the variables in the program.

#ifndef VARIABLE_H
#define VARIABLE_H

#include "DirectedGraph.h"
#include "Operation.h"
#include <vector>
#include <functional>


class Variable
{
  using vref = Variable*;
private:
  
  const Operation& m_operation{};

  
protected:

  /**
   * @brief Constructor for the Variable object.
   * @param operation The opeation that created the variable.
   */
  Variable(const Operation& operation) : m_operation{ operation }
  {}
  // The constructor is protected because Variables are not supposed to instantiated on their own.

  
public:
  
  /**
   * @brief Gets the consumers of this variable in the graph.
   * @param graph Directed graph from which the consumers are given.
   * @return Reference to the vector of variable references.
   */
  const std::vector<vref>& getConsumers(const DirectedGraph<vref>& graph)
  { return graph.getNodeConsumers(this); }

  
 /**
  * @brief Gets the inputs of this variable in the graph.
  * @param graph Directed graph from which the inputs are given.
  * @return Reference to the vector of variable references.
  */
  const std::vector<vref>& getInputs(const DirectedGraph<vref>& graph)
  { return graph.getNodeConsumers(this); }

  
  /**
   * @brief Gets the operation that created the variable.
   * @return Operation
   */
  const Operation& getOperation() const
  { return m_operation; }


  // Overloaded operator<<
  friend std::ostream& operator<<(std::ostream& out, const Variable& v)
  {
    return v.print(out);
  }

  
  // We'll rely on member function print() to do the actual printing
  // Because print() is a normal member function, it can be virtualized
  virtual std::ostream& print(std::ostream& out) const
  {
    out << "Variable<" << m_operation << '>';
    return out;
  }
};

#endif
