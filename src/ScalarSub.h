
#ifndef SCALAR_SUB_H
#define SCALAR_SUB_H

#include "OperationBinary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>

using Gradient = std::vector<double>;

class ScalarSub final : public OperationBinary
{
private:
  static constexpr std::string_view m_name{"ScalarSub"};
  
public:
  /**
   * @brief Operator for the functor
   * @param input1
   * @param input2
   * @return Unique ptr to resulting scalar.
   */
  std::unique_ptr<Scalar> operator()(const Scalar& input1, const Scalar& input2) const;

  /**
   * @brief Operator for the functor which takes a graph.
   * @param input1
   * @param input2
   * @param graph
   * @return Unique ptr to resulting scalar.
   * @note A raw ptr is also added to the graph.
   */
  std::unique_ptr<Scalar> operator()(Scalar& input1, Scalar& input2,
				     DirectedGraph<Variable*>& graph) const;
  
  void bop(const Variable& input1, const Variable& input2, Variable& variable) const override;

  Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
		 const Gradient& gradient) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
