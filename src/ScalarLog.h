
#ifndef SCALAR_LOG_H
#define SCALAR_LOG_H

#include "OperationUnary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>
#include <vector>

using Gradient = std::vector<double>;

class ScalarLog final : public OperationUnary
{
private:
  static constexpr std::string_view m_name{"ScalarLog"};

public:
  std::unique_ptr<Variable> operator()(const Variable& input) const;

  std::unique_ptr<Variable> operator()(DirectedGraph<Variable*>& graph, Variable& input) const;
  void uop(const Variable& input, Variable& variable) const override;

  Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
		 const Gradient& gradient) const override;

  std::ostream& print(std::ostream& out) const override;
};

#endif
