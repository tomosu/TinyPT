#include <cmath>
#include <cassert>
#include <iostream>
#include "MiscCalculation.h"
#include "GeometricProperty.h"
#include "Shape.h"
#include "ShapeImpl.h"




//////////////////////////
//
// Shape (Interface)
//
//////////////////////////
Shape::Shape()
{};

Shape::~Shape()
{};


SharedShapePtr Shape::Create(const ShapeParameter_t &parameter)
{
  if (parameter.Type == "Ball"){

    SharedShapePtr obj ( new Ball(parameter.Center, parameter.Radius) );
    return obj;
  }else{
    assert(!"Invalid Shape Type");
  }
  
};


bool Shape::GetIntersectionPoints(const Ray &ray, PointVector_t &node1, PointVector_t &node2)
{};

bool Shape::GetIntersectionPointsRayParameter(const Ray &ray, double &t1, double &t2)
{};


bool Shape::IsInside(const PointVector_t &p)
{};



//////////////////////////
//
//  Ball
// 
//////////////////////////
Ball::Ball(const PointVector_t &center, const double rad) : center(center), rad(rad)
{};


Ball::~Ball()
{};


inline bool Ball::GetIntersectionPoints(const Ray &ray, PointVector_t &node1, PointVector_t &node2)
{

  PointVector_t v =Vsub(this->center, ray.GetSource()); //vector: ray's source -> ball's center.
  PointVector_t vd =Scalar( Vsub(ray.GetTarget(), ray.GetSource()), 1.0/std::sqrt(Norm2(ray)) ); //vector: unit ray direction
   
  //coef of quadratic equation
  double a =1;
  double b = -2.0 *Dot(v, vd);
  double c = Dot(v,v) -(this->rad *this->rad);

  if( Det(a,b,c) >= 0.0 ){
    //smaller
    double t1 =QuadraticEquationRootSmaller(a,b,c);
    node1 = Vadd(ray.GetSource(), Scalar(vd, t1));

    //bigger
    double t2 =QuadraticEquationRootBigger(a,b,c);
    node2 = Vadd(ray.GetSource(), Scalar(vd, t2));
    
    return true;
  }
  return false;
}



inline bool Ball::GetIntersectionPointsRayParameter(const Ray &ray, double &t1, double &t2)
{

  PointVector_t v =Vsub(this->center, ray.GetSource()); //vector: ray's source -> ball's center.
  PointVector_t vd =Scalar( Vsub(ray.GetTarget(), ray.GetSource()), 1.0/std::sqrt(Norm2(ray)) ); //vector: unit ray direction
   
  //coef of quadratic equation
  double a =1;
  double b = -2.0 *Dot(v, vd);
  double c = Dot(v,v) -(this->rad *this->rad);

  if( Det(a,b,c) >= 0.0 ){
    
    //smaller
    t1 =QuadraticEquationRootSmaller(a,b,c);

    //bigger
    t2 =QuadraticEquationRootBigger(a,b,c);
    
    return true;
  }
  return false;
}




inline bool Ball::IsInside(const PointVector_t &p)
{
  if(rad*rad < Norm2(p, this->center) ){
    return false;
  }
  return true;
}
