#pragma once
#include <cmath>
#include <vector>
#include <iostream>
#include "GeometricProperty.h"



class Quaternion
{

public:

  Quaternion(){};

  Quaternion(const PointVector_t &axis, const double radian)
  {
    const double EPSILON_05 =0.000001;
    double norm2 =Dot(axis, axis);
    if(norm2 <EPSILON_05){
      std::cerr << "Invalid axis norm." << std::endl;
      exit(0);
    }
    PointVector_t naxis =Scalar(axis, 1.0/sqrt(norm2) );
    
    Q.clear();
    Q.push_back( cos( radian *0.5) );
    double tmp =sin( radian *0.5);
    Q.push_back(naxis.x *tmp);
    Q.push_back(naxis.y *tmp);
    Q.push_back(naxis.z *tmp);
  };


  Quaternion(const PointVector_t &point)
  {
    Q.clear();
    Q.push_back(0.0);
    Q.push_back(point.x);
    Q.push_back(point.y);
    Q.push_back(point.z);
  }


  Quaternion(const double t, const double x, const double y, const double z)
  {
    Q.clear();
    Q.push_back(t );
    Q.push_back(x );
    Q.push_back(y );
    Q.push_back(z );
  };


  Quaternion operator*(const Quaternion &obj) const
  {
    double t =Q[0] *obj.Q[0]
      -Q[1] *obj.Q[1]
      -Q[2] *obj.Q[2]
      -Q[3] *obj.Q[3];

    double x =Q[0] *obj.Q[1]
      +Q[1] *obj.Q[0]
      +Q[2] *obj.Q[3]
      -Q[3] *obj.Q[2];

    double y =Q[0] *obj.Q[2]
      +Q[2] *obj.Q[0]
      +Q[3] *obj.Q[1]
      -Q[1] *obj.Q[3];

    double z =Q[0] *obj.Q[3]
      +Q[3] *obj.Q[0]
      +Q[1] *obj.Q[2]
      -Q[2] *obj.Q[1];
      
    Quaternion ret(t,x,y,z);
    return ret;
  }


  Quaternion MakeConjugate() const
  {
    Quaternion ret( Q[0], -Q[1], -Q[2], -Q[3]);
    return ret;
  }

  void Print() const
  {
    std::cout << "(" << Q[0] << "," << Q[1] << "," << Q[2] << "," << Q[3] << ")" << std::endl;
  }

  std::vector<double> Q;

};



class Rotor
{
 public:

  Rotor(const PointVector_t &axis, double radian)
  {
    Quaternion q1(axis, radian);
    Q1 =q1;
    Q2 =q1.MakeConjugate();
  }


  PointVector_t Rotate(const PointVector_t &in) const
  {
    Quaternion p1(in);
    Quaternion p2 =Q2 *p1 *Q1;
    return Quaternion2PointVector(p2);
  }
  

  PointVector_t Quaternion2PointVector(const Quaternion &in) const
  {
    PointVector_t ret;
    ret.x =in.Q[1];
    ret.y =in.Q[2];
    ret.z =in.Q[3];
    return ret;
  }


 private:
  Quaternion Q1;
  Quaternion Q2;

};
