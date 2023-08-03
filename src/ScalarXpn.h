
#ifndef SCALAR_XPN_H
#define SCALAR_XPN_H

#include "OperationBinary.h"
#include "Scalar.h"
#include "DirectedGraph.h"

#include <memory>
#include <string_view>
#include <iostream>

using Gradient = std::vector<double>;


class ScalarXpn final : public OperationBinary
{
private:
  static constexpr std::string_view m_name{"ScalarXpn"};
  
public:
  std::unique_ptr<Scalar> operator()(const Scalar& base, const Scalar& exponent) const;

  std::unique_ptr<Scalar> operator()(DirectedGraph<Variable*>& graph,
				     Scalar& base, Scalar& exponent) const;
  
  void bop(const Variable& base, const Variable& exponent, Variable& variable) const override;

  Gradient bprop(const std::vector<Variable*>& inputs, const Variable& diff_var,
		 const Gradient& gradient) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
