
#ifndef SCALAR_EXP_H
#define SCALAR_EXP_H

#include "OperationUnary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>
#include <vector>

using Gradient = std::vector<double>;

class ScalarExp final : public OperationUnary
{
private:
  static constexpr std::string_view m_name{"ScalarExp"};
   
public:
  /**
   * @brief Operator for the functor
   * @param input
   * @return Unique ptr to resulting scalar.
   */
  std::unique_ptr<Scalar> operator()(const Scalar& input) const;

  /**
   * @brief Operator for the functor which takes a graph.
   * @param graph
   * @param input
   * @return Unique ptr to resulting scalar.
   * @note A raw ptr is also added to the graph.
   */
  std::unique_ptr<Scalar> operator()(DirectedGraph<Variable*>& graph, Scalar& input) const;
  void uop(const Variable& input, Variable& variable) const override;

  Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
		 const Gradient& gradient) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
