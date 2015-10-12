#include <numeric>
#include <cmath>
#include "Photon.h"
#include "Compound.h"
#include "GeometricProperty.h"
#include "CoordinateTransform.h"


double const EPSILON_05=0.000001;

//////////////////
//
// Photon
//
//////////////////
Photon::Photon(const double energy_MeV, const PointVector_t &direction)
  :IsAbsorbedFlag(false), Energy_MeV(energy_MeV), TransferedEnergy_MeV(0.0), Incline_rad(0.0)
{
  double norm =sqrt( Dot(direction, direction) );
  if(norm < EPSILON_05){
    std::cerr << "Invalid Direction Vector" << std::endl;
    exit(1);
  }
  this->Direction =Scalar(direction, 1.0/norm);
};



Photon::~Photon()
{};



//////////////////
// Factory
//////////////////
SharedPhotonPtr Photon::CreatePhoton(const double energy_MeV, const PointVector_t &direction)
{
  SharedPhotonPtr ret(new Photon(energy_MeV, direction));
  return ret;
};



/////////////////
// Freepath
/////////////////
double Photon::CalcFreePath_cm(const SharedCompoundPtr compound,
				      UniformRandomGenerator0to1 &random)
{
  
  PhotonCrossSection_t pc =compound->GetComposedPhotonCrossSectionByEnergy( this->Energy_MeV );
  double mu_per_rho = pc.MassAbsorptionCoefficient_cm2_per_gram;
  double mu = mu_per_rho * compound->GetDensity_gram_per_cm3();
  double freePathLen_cm = -log( random.GenRand() )/mu;

  return freePathLen_cm;
};



//////////////////
// Determine Interaction
//////////////////
PhotonInteraction Photon::DetermineInteraction(const SharedCompoundPtr compound,
						      UniformRandomGenerator0to1 &random)
{
  PhotonCrossSection_t pc =compound->GetComposedPhotonCrossSectionByEnergy(this->Energy_MeV);

  //Calc cumulative distribution
  double photoele = pc.PhotoElectricCrossSection_cm2_per_gram;
  double coherent = photoele +pc.CoherentScatteringCrossSection_cm2_per_gram;
  double total = coherent +pc.IncoherentScatteringCrossSection_cm2_per_gram;
  double rand =random.GenRand();

  if(photoele/total > rand){ return PHOTO_ELECTRIC;}
  if(coherent/total > rand){ return COHERENT_SCATTERING;}
  return INCOHERENT_SCATTERING;  
};




/////////////////////
//
// Photo Electric
//
/////////////////////
inline double FluorescenceYield(const int Z)
{
  const double a_k =1.12E6; //
  double omega_k =1.0/( 1.0 + a_k/(double)(Z*Z*Z*Z) );
  return omega_k;
}



inline bool IsCapturedAtom(const double rand, const double cumulativeDist)
{
  return rand <= cumulativeDist;
}



MontecarloResult Photon::PhotoElectric(const SharedCompoundPtr compound,
				       UniformRandomGenerator0to1 &random)
{

  ComposedK_alpha k_a =compound->GetComposedK_alphaByEnergy( this->Energy_MeV );
  
  ////////////////////////////////
  // Which atom captures photon ?
  ////////////////////////////////
  double probSum =0.0;
  for(int i=0; i<k_a.size(); i++){ probSum +=k_a[i].RelativeProbability; }

  double rand1 =random.GenRand();

  int capturedIdx;
  double cumulativeDist=0.0;

  capturedIdx =k_a.size()-1;
  for(int i=0; i <k_a.size(); i++){
    cumulativeDist +=k_a[i].RelativeProbability /probSum;
    if( IsCapturedAtom(rand1, cumulativeDist) ){
      capturedIdx =i;
      break;
    }
  }


  /////////////////
  // Fluorescent ?
  /////////////////

  // Energy check
  if( this->Energy_MeV < k_a[capturedIdx].K_alpha_MeV ){
    
    //Not fluorescent
    this->IsAbsorbedFlag =true;
    this->TransferedEnergy_MeV =this->Energy_MeV;
    this->Energy_MeV=0.0;
    return MC_ACCEPTED;
  }

  // Probability check
  double rand2 =random.GenRand();  
  if( rand2 <= FluorescenceYield(k_a[capturedIdx].Z) ) {
    
    //Fluorescent
    this->IsAbsorbedFlag =false;
    this->TransferedEnergy_MeV =this->Energy_MeV -k_a[capturedIdx].K_alpha_MeV;
    this->Energy_MeV =k_a[capturedIdx].K_alpha_MeV;
    this->Direction =RandomOnUnitSphere(random);
    return MC_ACCEPTED;

  }else{

    //Not fluorescent
    this->IsAbsorbedFlag =true;
    this->TransferedEnergy_MeV =this->Energy_MeV;
    this->Energy_MeV=0.0;
    return MC_ACCEPTED;
    
  }
}



/////////////////////
//
// Coherent(Rayleigh) Scattering
//
/////////////////////
double ConvertMeVToAngstroms(const double energy_MeV)
{
  double energyKeV = energy_MeV *1000;
  double energy_angstroms;

  if(energyKeV > 0.0){
    energy_angstroms = 12.4/energyKeV; // Based on formula: E = hc/lambda
  }else{
    energy_angstroms = 0.0;
  }
  return energy_angstroms;
};



MontecarloResult Photon::CoherentScattering(const SharedCompoundPtr compound,
					    UniformRandomGenerator0to1 &random)
{
  double lambda = ConvertMeVToAngstroms(this->Energy_MeV);

  double r1 =compound->GetComposedCoherentScatteringFormFactorDistributionMax() *random.GenRand();
  double x =compound->GetMomentumTransferByComposedCoherentScatteringFormFactorDistribution( r1 );

  double sinValHalf = x *lambda;
  double cosVal = 1.0 -2.0 *sinValHalf *sinValHalf;
  double r2 = 2.0 *random.GenRand();

  if(fabs(cosVal)<1.0){
    if(r2 <= 1 +cosVal *cosVal){

      SphericalCoordinateAngle_t displace;
      this->Incline_rad =displace.Inclination =acos(cosVal);
      displace.Azimuth =2.0 *M_PI *random.GenRand();    
      this->Direction =ChangeDirectionBySphericalCoordinateAngle(this->Direction, displace);

      return MC_ACCEPTED;
    }
  }
  
  return MC_REJECTED;
}




/////////////////////
//
// Incoherent(Compton) Scattering
//
/////////////////////
MontecarloResult Photon::IncoherentScattering(const SharedCompoundPtr compound,
					      UniformRandomGenerator0to1 &random)
{
  const double ElectronRestEnergy_MeV =0.511;
  double EE_keV =ElectronRestEnergy_MeV *1000.0;

  // Kahn's method
  double E0_keV =this->Energy_MeV *1000.0;
  double alpha =E0_keV /EE_keV;
  double A =(2.0 *alpha +1.0) /(2.0 *alpha +9.0);
  double r1 =random.GenRand();
  double r2 =random.GenRand();
  double r3 =random.GenRand();
  double eta;

  if(r1 <= A){

    eta =1.0 +2.0 *alpha *r2;
    double D =4.0 *( 1.0/eta -1.0/(eta*eta) );
    if(r3 > D){
      return MC_REJECTED;
    }

  }else{

    eta =(1.0 +2.0 *alpha)/(1 +2 *alpha *r2);
    double tmp = 1.0 -eta/alpha +1.0/alpha;
    double E =( tmp *tmp +1.0/eta ) /2.0;
    if(r3 > E){
      return MC_REJECTED;
    }
  }

  double E1_keV =E0_keV /eta;
  double cosVal =1.0 -EE_keV *(E0_keV -E1_keV) /(E1_keV *E0_keV);
  double sinValHalf =sqrt( (1.0 -cosVal)/2 );
  double x =sinValHalf *E0_keV /12.4; // x: Momentum transfer

  double Sx =compound->GetComposedIncoherentScatteringFormFactorByMomentumTransfer(x);
  double Smax =compound->GetComposedIncoherentScatteringFormFactorMax();
  double r4 =Smax *random.GenRand();
  
  if(r4 > Sx){ return MC_REJECTED; }

  // Update energy
  this->TransferedEnergy_MeV =(E0_keV -E1_keV) /1000.0;
  this->Energy_MeV =E1_keV /1000.0;

  // Change direction
  SphericalCoordinateAngle_t displace;
  this->Incline_rad =displace.Inclination =acos(cosVal);
  displace.Azimuth =2.0 *M_PI *random.GenRand();
  this->Direction =ChangeDirectionBySphericalCoordinateAngle(this->Direction, displace);

  return MC_ACCEPTED;  
}
