
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class InvalidOperationException : public std::runtime_error
{
public:
  /**
   * @brief Exception to be called when an operation has invalid inputs.
   * @param error String with a relevant error message.  
   */
  InvalidOperationException(const std::string& error)
    : std::runtime_error{ error.c_str() }
  {}
};

class BaseException : public std::runtime_error
{
public:
  /**
   * @brief Exception to be called when an operation has invalid inputs.
   * @param error String with a relevant error message.  
   */
  BaseException(const std::string& error)
    : std::runtime_error{ error.c_str() }
  {}
};

class BadWalkException : public std::runtime_error
{
public:
  /**
   * @brief Exception to be called when the walk over a computational graph fails.
   * @param error String with a relevant error message.  
   */
  BadWalkException(const std::string& error)
    : std::runtime_error{ error.c_str() }
  {}
};



#endif
