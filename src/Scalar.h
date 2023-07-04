
// Scalar class for scalars used with the operations to create coumputational graphs.

#ifndef SCALAR.H
#define SCALAR.H

class Scalar
{
 private:
  double m_value {};
  static constexpr m_dimension{ 0 };

 public:
  // Constructor takes a value for the scalar. Default constructor will have a value of zero.
  Scalar(double value);

  // Function that sets the value of the Scalar object.
  void setValue(double value) { m_value = value; };

  // Returns the value of the scalar
  double getValue() const { return m_value; };

  // Returns the dimension of a scalar, which trivialli is 0.
  int getDimension() const { return m_dimension};
};

#endif
