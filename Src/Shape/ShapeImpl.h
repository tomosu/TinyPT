#pragma once

#include "GeometricProperty.h"
#include <tr1/memory>
#include "Shape.h"


class Ball : public Shape
{

public:

  Ball(const PointVector_t&, const double);
  ~Ball();

  bool GetIntersectionPoints(const Ray&, PointVector_t&, PointVector_t&);
  bool GetIntersectionPointsRayParameter(const Ray&, double &t1, double &t2);
  bool IsInside(const PointVector_t&);

private:
  PointVector_t center;
  double rad;

};
