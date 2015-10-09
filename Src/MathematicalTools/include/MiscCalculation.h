#pragma once

#include <cmath>



////////////////////////////
// Quadratic formula
////////////////////////////
inline double Det(const double a, const double b, const double c) // determinant
{
  return b*b -4*a*c;
}


inline double QuadraticEquationRootSmaller(const double a, const double b, const double c) // less root
{
  return ( -b -sqrt(Det(a,b,c)) )/(2*a);
}


inline double QuadraticEquationRootBigger(const double a, const double b, const double c) // greater root
{
  return ( -b +sqrt(Det(a,b,c)) )/(2*a);
}






////////////////////////////
// Compliment
////////////////////////////
inline double LogComplement(const double x0,
			    const double x1,
			    const double y0,
			    const double y1,
			    const double x) // x0 < x < x1
{
  double log_y0 = log(y0);
  double log_y1 = log(y1);
 
  return exp( (x-x0) *(log_y1-log_y0) /(x1-x0) +log_y0 );
}


inline double LinearComplement(const double x0,
			       const double x1,
			       const double y0,
			       const double y1,
			       const double x)
{
  return y0 +(x-x0)*(y1-y0)/(x1-x0);
}



///////////////////////////
// Measure
///////////////////////////
inline double TrapezoidalArea(const double x0, const double x1, const double y0, const double y1)
{
  return (x1-x0)*(y1+y0)/2.0;
}
