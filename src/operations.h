
#ifndef OPERATIONS_H
#define OPERATIONS_H


#include "OperationBinary.h"
#include "OperationUnary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>


class ScalarAdd : public OperationBinary
{
private:
  static constexpr std::string_view m_name{"ScalarAdd"};
public:
  /**
   * @brief Operator for the functor
   * @param input1
   * @param input2
   * @return A new scalar with the value of input1 and input2 and reference to this operation.
   * @note the object will be returned by value.
   */
  std::unique_ptr<Scalar> operator()(const Scalar& input1, const Scalar& input2) const
  {
    return std::make_unique<Scalar>(*this, input1.getValue() + input2.getValue());
  }

  /**
   * @brief Operator for the functor which takes a graph.
   * @param input1
   * @param input2
   * @param graph
   * @return A reference to a new scalar with the value of input1 and input2.
   * @note A shared_ptr is also added to the graph.
   */
  std::unique_ptr<Scalar> operator()(Scalar& input1, Scalar& input2,
				     DirectedGraph<Variable*>& graph) const
  {
    // Create the resulting scalar on the heap.
    auto res{ std::make_unique<Scalar>(*this, input1.getValue() + input2.getValue()) };

    // Add connections to the graph
    graph.addConnection(&input1, res.get());
    graph.addConnection(&input2, res.get());
    
    return res;
  }
  
  /**
   * @brief Performs the forward operation.
   * @param input1, input2 Input variable node to a binary operation in a computational graph.
   * @param variable Output variable node in a computational graph.
   */
  void op(const Scalar& input1, const Scalar& input2, Scalar& variable) const
  { variable.setValue(input1.add(input2)); }

  
  std::ostream& print(std::ostream& out) const override
  {
    out << m_name;
    return out;
  }
};


class Input : public OperationUnary
{
private:
  static constexpr std::string_view m_name{"Input"};
public:
  void op(const Variable& input, Variable& variable)
  { /* Do nothing since this is the input. */ }

  std::ostream& print(std::ostream& out) const override
  {
    out << m_name;
    return out;
  }
};


#endif

// Recall that virtual functions are functions that resolve into the most derived class.
