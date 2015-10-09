#pragma once

#include "GeometricProperty.h"
#include <tr1/memory>
#include <string> 

class Shape;
typedef std::tr1::shared_ptr<Shape> SharedShapePtr;


struct ShapeParameter_t
{
  std::string Type;
  PointVector_t Center;
  double Radius;
  double Length;
};


class Shape
{
 public:
  Shape();
  virtual ~Shape();
  
  static SharedShapePtr Create(const ShapeParameter_t &parameter);

  virtual bool GetIntersectionPoints(const Ray&, PointVector_t&, PointVector_t&);
  virtual bool GetIntersectionPointsRayParameter(const Ray&, double &t1, double &t2);
  virtual bool IsInside(const PointVector_t&);

};
