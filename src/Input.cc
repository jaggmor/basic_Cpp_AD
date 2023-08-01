
#include "Input.h"
#include "Exceptions.h"

void Input::uop(const Variable& input, Variable& variable) const
{
  throw InvalidOperationException("Input does not have operation capabilities.");
}

std::ostream& Input::print(std::ostream& out) const
{
  out << m_name;
  return out;
}
