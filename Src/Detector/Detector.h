#pragma once
#include "FlyingPhoton.h"
#include "GeometricProperty.h"
#include <tr1/memory>

class Detector;
typedef std::tr1::shared_ptr<Detector> SharedDetectorPtr;


struct DetectorParameter_t
{
  std::string Type;
  PointVector_t NormalVector;
  PointVector_t UpVector;
  PointVector_t Center;
  double PitchU;
  double PitchV;
  long long int SizeU;
  long long int SizeV;
};


class Detector
{
public:
  Detector();
  virtual ~Detector();

  static SharedDetectorPtr Create(const DetectorParameter_t &);

  virtual void Clear();
  virtual bool Detect(const SharedFlyingPhotonPtr &);
  virtual bool PrintDetectionInfo(const SharedFlyingPhotonPtr &);
  virtual bool Export(const std::string &filepath, const double gain);
  virtual bool Export(std::vector<double> &out);
  
};
