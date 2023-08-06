
#ifndef INPUT_H
#define INPUT_H


#include "OperationUnary.h"
#include "Variable.h"

#include <string_view>
#include <iostream>



class Input : public OperationUnary
{
private:
  static constexpr std::string_view m_name{"Input"};

public:
  virtual std::unique_ptr<Variable> operator()(const Variable& input) const override;
  
  virtual std::unique_ptr<Variable> operator()(DirectedGraph<Variable*>& graph, Variable& input) const override;

  void uop(const Variable& input, Variable& variable) const override;  

  Gradient bprop(const std::vector<Variable*>& inputs,
		 const Variable& diff_var, const Gradient& gradient) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
