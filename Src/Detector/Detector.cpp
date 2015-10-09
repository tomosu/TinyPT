#include <cassert>
#include <cmath>
#include <iostream>
#include "Detector.h"
#include "DetectorImpl.h"
#include "FileUtil.h"

const double EPSILON_05 =0.000001;
const double EPSILON_02 =0.001;


/////////////////////
//
// Detector (interface)
//
/////////////////////
Detector::Detector()
{};

Detector::~Detector()
{};

SharedDetectorPtr Detector::Create(const DetectorParameter_t &parameter)
{
  if(parameter.Type == "PlaneIdealDetector"){
    SharedDetectorPtr ret(new PlaneIdealDetector(parameter) );
    return ret;
  }else{
    assert(!"Invalid Detector Type");
  }
};

void Detector::Clear()
{};

bool Detector::Detect(const SharedFlyingPhotonPtr &photon)
{
  return false;
};

bool Detector::PrintDetectionInfo(const SharedFlyingPhotonPtr &photon)
{
  return false;
};
  
bool Detector::Export(const std::string &filepath, const double gain)
{
  return false;
};

bool Detector::Export(std::vector<double> &out)
{
  return false;
};




/////////////////////
//
// Plane ideal detector
//
/////////////////////
PlaneIdealDetector::PlaneIdealDetector(const DetectorParameter_t &in )
  : Center(in.Center), PitchU(in.PitchU), PitchV(in.PitchV),
    SizeU(in.SizeU), SizeV(in.SizeV)
{

  double norm =sqrt( Dot(in.NormalVector, in.NormalVector) );
  if(norm < EPSILON_05){
    std::cerr << "Invalid Normal Vector" << std::endl;
    exit(1);
  }
  this->NormalVector =Scalar(in.NormalVector, 1.0/norm);

  double normu =sqrt( Dot(in.UpVector, in.UpVector) );
  if(normu < EPSILON_05){
    std::cerr << "Invalid Up Vector" << std::endl;
    exit(1);
  }
  this->VVector =Scalar(in.UpVector, 1.0/normu);

  if(fabs(Dot(NormalVector, VVector)) >EPSILON_05){
    std::cerr << Dot(NormalVector, VVector) << std::endl;
    std::cerr << "Dot(NormalVector, UpVector) is not 0" << std::endl;
    exit(1);
  }

  this->UVector =Cross(this->NormalVector, this->VVector);
  
  this->Clear();
};


PlaneIdealDetector::~PlaneIdealDetector()
{};


void PlaneIdealDetector::Clear()
{
  this->DetectorPixel.clear();
  for(int i=0; i<this->SizeU*this->SizeV; i++){
    DetectorPixel.push_back(0.0);
  }
}


bool PlaneIdealDetector::GetPlaneIntersectionNode(const SharedFlyingPhotonPtr &photon, PointVector_t &node)
{

  PointVector_t vn =this->NormalVector;
  PointVector_t pr =photon->GetDirection();
  if(fabs(Dot(vn,pr))<EPSILON_05){ //parallel ray
    return false;
  }

  PointVector_t pp =photon->GetCurrentPosition();
  double d = Dot(this->Center, this->NormalVector);
  double t = -(vn.x*pp.x +vn.y*pp.y +vn.z*pp.z -d) /(vn.x*pr.x +vn.y*pr.y +vn.z*pr.z);
  if(t < 0.0){return false;}
  
  node.x =t*pr.x +pp.x;
  node.y =t*pr.y +pp.y;
  node.z =t*pr.z +pp.z;

  return true;
};


PlaneDetectorCoordinate_t PlaneIdealDetector::GetPixelCoordinate(const PointVector_t &node)
{
  
  double u =Dot(node, this->UVector);
  double v =Dot(node, this->VVector);
  
  double detCu =Dot(this->Center, this->UVector);
  double detCv =Dot(this->Center, this->VVector);
  
  double detOu = detCu -(this->SizeU *this->PitchU)/2.0;
  double detOv = detCv -(this->SizeV *this->PitchV)/2.0;
  
  PlaneDetectorCoordinate_t ret;
  ret.u= (int)floor( (u -detOu) /this->PitchU );
  ret.v= (int)floor( (v -detOv) /this->PitchV );

  return ret;
};



bool PlaneIdealDetector::Detect(const SharedFlyingPhotonPtr &photon)
{
  PointVector_t node;

  if( !this->GetPlaneIntersectionNode(photon, node) ){
    return false;
  }

  PlaneDetectorCoordinate_t p=this->GetPixelCoordinate(node);
  if( !this->IsValidPixelPosition(p) ){
    return false;
  }

  long long int idx =p.u +p.v*SizeU;
  if(photon->GetEnergy_MeV() > 0.0001){
    this->DetectorPixel[idx] += 0.01;
  }
  return true;
};




bool PlaneIdealDetector::Export(const std::string &filepath, const double gain)
{
  return SavePixelDoubleToInt(filepath, this->DetectorPixel, gain);
};


bool PlaneIdealDetector::Export(std::vector<double> &out)
{
  int size=this->DetectorPixel.size();
  out.clear();
  out.resize(size, 0.0);
  for(int i=0; i<size; i++){
    out[i]=DetectorPixel[i];
  }
  return true;
};



bool PlaneIdealDetector::PrintDetectionInfo(const SharedFlyingPhotonPtr &photon)
{
  PointVector_t node;

  if( !this->GetPlaneIntersectionNode(photon, node) ){
    std::cout << "Fly Away(other direction)" <<std::endl;
    return false;
  }
  std::cout << "point(" << node.x << "," << node.y << "," << node.z << ")" << std::endl;
  
  PlaneDetectorCoordinate_t p=this->GetPixelCoordinate(node);
  if( !this->IsValidPixelPosition(p) ){
    std::cout << "Fly away(out of flame)" <<std::endl;
    return false;
  }

  std::cout << "Hit:" << "pixel(" << p.u << "," << p.v << ")";
  return true;
};

