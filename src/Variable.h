
// An inheritable class for the variables in the program.

#ifndef VARIABLE_H
#define VARIABLE_H

#include "DirectedGraph.h"
#include "Operation.h"
#include <vector>
#include <functional>
#include <memory>
#include <string>


class Variable
{
  using vref = Variable*;
private:
  const std::string m_name{""};
  const Operation& m_operation;
  bool m_flag{};

protected:
  std::unique_ptr<double> m_memory{}; // initialized as nullptr
  std::vector<int> m_lengths{}; 
  /**
   * @param name Named variables are usually inputs.
   * @param operation The opeation that created the variable.
   * @param flag
   */
  Variable(const std::string& name, const Operation& operation, bool flag=false)
    : m_name{ name }
    , m_operation{ operation }
    , m_flag{ flag }
  {}
  Variable(const Operation& operation, bool flag=false)
    : m_operation{ operation }
    , m_flag{ flag }
  {}
  
  
public:
  virtual void setValue(double value) = 0;
  virtual double getValue() const = 0;      
  /**
   * @brief Redirects the memory pointed to, deleting it in the process, to another memory block.
   * @param 
   */
  void overwriteMemory(std::unique_ptr<double> new_memory, std::vector<int> new_lengths)
  {
    m_memory = std::move(new_memory);
    m_lengths = new_lengths;
  }
  /**
   * @param memory Memory that is to be copied to m_memory.
   */
  //virtual void copyInto(const double* memory) = 0;    

  /**
   * @return A pointer to the memory the object is holding.
   */
  double* getMemoryPtr() const
  { return m_memory.get(); }

  /**
   * @brief The lengths of the variable, entries depend on type.
   * @return Vector of lengts, i.e. {2, 2, 2} for a cubic 3D matrix.
   */
  const std::vector<int>& getLengths() const
  { return m_lengths; }
  
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
  { return graph.getNodeInputs(this); }  
  
  /**
   * @brief Gets the operation that created the variable.
   * @return Operation
   */
  const Operation& getOperation() const
  { return m_operation; }

  const std::string& getName() const
  { return m_name; }
  
  void setTrue()
  { m_flag = true; }

  void setFalse()
  { m_flag = false; }

  operator bool() const
  { return m_flag; }

  bool operator! () const
  { return !m_flag; }
  
  // Overloaded operator<<
  friend std::ostream& operator<<(std::ostream& out, const Variable& v)
  {
    return v.print(out);
  }
  
  // We'll rely on member function print() to do the actual printing
  // Because print() is a normal member function, it can be virtualized
  virtual std::ostream& print(std::ostream& out) const = 0;
  
};

#endif
