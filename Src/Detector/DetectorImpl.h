#pragma once
#include "Detector.h"


struct PlaneDetectorCoordinate_t{
  int u;
  int v;
};


class PlaneIdealDetector : public Detector
{
public:
  PlaneIdealDetector(const DetectorParameter_t &);
  ~PlaneIdealDetector();

  
  void Clear();
  bool Detect(const SharedFlyingPhotonPtr &);
  bool PrintDetectionInfo(const SharedFlyingPhotonPtr &);
  bool Export(const std::string &filepath, const double gain);
  bool Export(std::vector<double> &);

private:
  
  bool GetPlaneIntersectionNode(const SharedFlyingPhotonPtr &photon, PointVector_t &node);
  PlaneDetectorCoordinate_t GetPixelCoordinate(const PointVector_t &node);
  
  bool IsValidPixelPosition(const PlaneDetectorCoordinate_t &in)
  {
    return in.u>=0 && in.u<this->SizeU && in.v>=0 && in.v<this->SizeV;
  };
  
  std::vector<double> DetectorPixel;
  PointVector_t NormalVector;
  PointVector_t UVector;
  PointVector_t VVector;
  PointVector_t Center;
  double PitchU;
  double PitchV;
  long long int SizeU;
  long long int SizeV;
};
