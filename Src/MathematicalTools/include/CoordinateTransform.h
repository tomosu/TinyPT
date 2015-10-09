#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include "GeometricProperty.h"


  
inline double Crop(const double min, const double max, const double val)
{
  return std::min(max, std::max(min, val));
}



inline SphericalCoordinateAngle_t ChangeAngle(const SphericalCoordinateAngle_t &origin,
					      const SphericalCoordinateAngle_t &displace)
{
  const double EPSILON_05=0.000001;
  
  double theta0 =origin.Inclination;
  double psi0 =origin.Azimuth;
  double thetaD =displace.Inclination;
  double psiD =displace.Azimuth;
  double cos_theta0 = cos(theta0);
  double sin_theta0 = sin(theta0);
  double cos_psi0 = cos(psi0);
  double sin_psi0 = sin(psi0);
  double cos_omega = cos(thetaD);
  double sin_omega = sin(thetaD);
  double cos_phi = cos(psiD);
  double sin_phi = sin(psiD);
  
  //calculate polar1, azimuth
  double cos_theta1 = cos_theta0 *cos_omega +sin_theta0 *sin_omega *cos_phi;
  double sin_theta1 = sin( acos( Crop(-1.0, 1.0, cos_theta1) ));
  
  SphericalCoordinateAngle_t ret;
  
  double sin_psi1_0 = sin(psiD) * sin_omega / sin_theta1;
  double cos_psi1_0 = (cos_omega -cos_theta0 *cos_theta1) / (sin_theta0 *sin_theta1);

  if(fabs(sin_theta1) > EPSILON_05 && fabs(sin_theta0) > EPSILON_05){
    double sin_psi1 = sin_psi0 * cos_psi1_0 + cos_psi0 * sin_psi1_0;
    double cos_psi1 = cos_psi0 * cos_psi1_0 - sin_psi0 * sin_psi1_0;
    
    ret.Inclination =acos( Crop(-1.0, 1.0, cos_theta1) );
    ret.Azimuth =atan2(sin_psi1, cos_psi1);

  }else{
    ret.Inclination =acos( Crop(-1.0, 1.0, cos_theta1) );
    ret.Azimuth =psiD;

  }
  return ret;
}




inline PointVector_t ConvertAngleToDirectionVector(const SphericalCoordinateAngle_t &in)
{
  double theta=in.Inclination;
  double psi=in.Azimuth;

  PointVector_t vd;
  double sin_theta =sin(theta);
  vd.x = sin_theta * cos(psi);
  vd.y = sin_theta * sin(psi);
  vd.z = cos(theta);

  return vd;
}



inline SphericalCoordinateAngle_t ConvertDirectionVectorToAngle(const PointVector_t & vd)
{
  double cos_theta =vd.z /sqrt(vd.x*vd.x + vd.y*vd.y +vd.z*vd.z);
  double theta_tmp =acos(Crop(-1.0, 1.0, cos_theta) );
  double psi_tmp =atan2(vd.y, vd.x);

  SphericalCoordinateAngle_t ret;
  ret.Inclination = theta_tmp;
  ret.Azimuth = psi_tmp;
  
  return ret;
}



inline PointVector_t ChangeDirectionBySphericalCoordinateAngle(const PointVector_t &origin,
							       const SphericalCoordinateAngle_t &displace)
{
  SphericalCoordinateAngle_t nd =ChangeAngle( ConvertDirectionVectorToAngle(origin), displace);
  return ConvertAngleToDirectionVector(nd);
}


