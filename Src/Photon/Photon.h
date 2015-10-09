#pragma once
#include <tr1/memory>
#include <algorithm>
#include "Compound.h"
#include "RandomNumberGenerator.h"
#include "GeometricProperty.h"

class Photon;
typedef std::tr1::shared_ptr<Photon> SharedPhotonPtr;


enum PhotonInteraction{
  PHOTO_ELECTRIC,
  COHERENT_SCATTERING,
  INCOHERENT_SCATTERING,
};


enum MontecarloResult{
  MC_ACCEPTED,
  MC_REJECTED,
};


///////////////////////
//
// Photon
//
///////////////////////
class Photon{

public:
  
  Photon(const double, const PointVector_t &);
  ~Photon();

  // Factory
  static SharedPhotonPtr CreatePhoton(const double energy_MeV, const PointVector_t &direction);


  // Interaction
  double CalcFreePath_cm(const SharedCompoundPtr compound,
			 UniformRandomGenerator0to1 &random);

  PhotonInteraction DetermineInteraction(const SharedCompoundPtr compound,
					 UniformRandomGenerator0to1 &random);

  MontecarloResult PhotoElectric(const SharedCompoundPtr compound,
				 UniformRandomGenerator0to1 &random);
    
  MontecarloResult CoherentScattering(const SharedCompoundPtr compound,
				      UniformRandomGenerator0to1 &random);
 
  MontecarloResult IncoherentScattering(const SharedCompoundPtr compound,
					UniformRandomGenerator0to1 &random);
  
  double GetTransferedEnergy_MeV(){ return this->TransferedEnergy_MeV; }

  double GetEnergy_MeV(){ return this->Energy_MeV; }

  PointVector_t GetDirection(){ return this->Direction; }

  double GetIncline_rad(){ return this->Incline_rad; }


protected:
  bool IsAbsorbedFlag;
  double Energy_MeV;
  double TransferedEnergy_MeV;
  double Incline_rad;
  PointVector_t Direction;
};
