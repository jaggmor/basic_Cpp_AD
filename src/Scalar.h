
// Scalar class for scalars used with the operations to create coumputational graphs.

#ifndef SCALAR_H
#define SCALAR_H

class Scalar
{
private:
  double m_value {};
  static constexpr int m_dimension{ 0 };
  
public:
  Scalar(double value=0.0);
  
  void setValue(double value) { m_value = value; };
  double getValue() const { return m_value; };
  
  constexpr int getDimension() const { return m_dimension; };
  
  Scalar add(const Scalar& rscalar) const;
  Scalar subtract(const Scalar& subtrahend) const;
  Scalar multiply(const Scalar& factor) const;
  Scalar divide(const Scalar& denominator) const;
  
};

#endif
