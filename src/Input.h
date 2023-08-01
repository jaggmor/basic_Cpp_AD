
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

  void uop(const Variable& input, Variable& variable) const override;
  
  std::ostream& print(std::ostream& out) const override;
};


#endif
