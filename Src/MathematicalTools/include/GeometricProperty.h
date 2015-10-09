#pragma once

#include <stdio.h>

//////////////////////////
// Polar coordinate
//////////////////////////
struct SphericalCoordinateAngle_t{
  double Inclination;
  double Azimuth;
};

//////////////////////////
// Point vector
//////////////////////////
struct PointVector_t{
  PointVector_t(){};
  PointVector_t(double x, double y, double z) : x(x), y(y), z(z) {};

  double x;
  double y;
  double z;
};



//////////////////////////
// Ray
//////////////////////////
class Ray{
  
public:
  Ray(){};
  Ray(PointVector_t &s, PointVector_t &t) : source(s), target(t){};
  ~Ray(){};

  void SetSource(const PointVector_t &s){source =s;};
  void SetTarget(const PointVector_t &t){target =t;};
  //const PointVector_t GetSource (PointVector_t &s) const {s=source;};
  //const PointVector_t GetTarget (PointVector_t &t) const {t=target;};
  const PointVector_t GetSource () const {return source;};
  const PointVector_t GetTarget () const {return target;};
  
private:
  PointVector_t source;
  PointVector_t target;
};






////////////////////////////
// Vector calculation
////////////////////////////
inline PointVector_t Vadd(const PointVector_t &a, const PointVector_t &b)
{
  PointVector_t ret;
  ret.x =a.x +b.x;
  ret.y =a.y +b.y;
  ret.z =a.z +b.z;
  return ret;
}



inline PointVector_t Vsub(const PointVector_t &a, const PointVector_t &b)
{
  PointVector_t ret;
  ret.x =a.x -b.x;
  ret.y =a.y -b.y;
  ret.z =a.z -b.z;
  return ret;
}


inline PointVector_t Scalar(const PointVector_t &a, const double k)
{
  PointVector_t ret;
  ret.x =a.x *k;
  ret.y =a.y *k;
  ret.z =a.z *k;
  return ret;
}



inline double Dot(const PointVector_t &a, const PointVector_t &b)
{
  double ret=0.0;
  ret +=a.x *b.x;
  ret +=a.y *b.y;
  ret +=a.z *b.z;
  return ret;
}

inline PointVector_t Cross(const PointVector_t &a, const PointVector_t &b)
{
  PointVector_t ret;
  ret.x =a.y*b.z -a.z*b.y;
  ret.y =a.z*b.x -a.x*b.z;
  ret.z =a.x*b.y -a.y*b.x;
  return ret;
}



inline double Norm2(const PointVector_t &a, const PointVector_t &b)
{
  double ret=0.0;
  PointVector_t diff = Vsub(a,b);
  return Dot(diff, diff);
}



inline double Norm2(const Ray &a)
{
  double ret=0.0;
  PointVector_t s,t;
  
  PointVector_t diff = Vsub(a.GetSource(), a.GetTarget());
  return Dot(diff, diff);
}


inline void PrintPoint(const PointVector_t &p)
{
  printf("(%e,%e,%e)", p.x, p.y, p.z);
}
