
#ifndef SCALAR_ABS_H
#define SCALAR_ABS_H

#include "OperationUnary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>
#include <vector>

using Gradient = std::vector<double>;

class ScalarAbs final : public OperationUnary
{
private:
  static constexpr std::string_view m_name{"ScalarAbs"};
   
public:
  std::unique_ptr<Scalar> operator()(const Scalar& input) const;

  std::unique_ptr<Scalar> operator()(DirectedGraph<Variable*>& graph, Scalar& input) const;
  void uop(const Variable& input, Variable& variable) const override;

  Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
		 const Gradient& gradient) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
