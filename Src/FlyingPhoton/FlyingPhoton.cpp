#include "FlyingPhoton.h"
#include "PhantomTree.h"
#include <cfloat>
#include <algorithm>
#include <assert.h>



FlyingPhoton::FlyingPhoton(const double energy_MeV, const PointVector_t &direction, const PointVector_t &currentPosition)
  :CurrentPosition(currentPosition), DoesFlyAwayFlag(false), Photon(energy_MeV, direction)
{
  
}



///////////////
// Factory
///////////////
SharedFlyingPhotonPtr FlyingPhoton::CreateFlyingPhoton(const double energy_MeV,
						       const PointVector_t &direction,
						       const PointVector_t &currentPosition)
{
  SharedFlyingPhotonPtr ret(new FlyingPhoton(energy_MeV, direction, currentPosition));
  return ret;
}



Ray FlyingPhoton::GetPhotonRay(double t)
{
  Ray r;
  r.SetSource( this->CurrentPosition );
  r.SetTarget( CalcNextPosition(t) );
  return r;
};




TransportResult FlyingPhoton::GetTransportResult(const double t)
{
  std::vector<WeakPhantomNodePtr> children =this->CurrentNode.lock()->GetChildren();
  Ray ray =this->GetPhotonRay(t);

  // Intersect with children node?
  bool haveNode=false;
  for(int i=0; i<children.size(); i++){
    double dummy;
    haveNode = haveNode || children[i].lock()->GetForwardNodeRayParameter(ray, dummy);
  }

  if(haveNode){
    return TRAVERSE_INTO_CHILD_NODE;
  }

  // Inside current node ?
  if( this->CurrentNode.lock()->IsInside( this->CalcNextPosition(t) ) ){
    return INTERACTION_INSIDE_NODE;
  }

  return GET_OUTSIDE_OF_NODE;
};




bool FlyingPhoton::GetNextChildNode(const double t, WeakPhantomNodePtr &out)
{
  std::vector<WeakPhantomNodePtr> children=this->CurrentNode.lock()->GetChildren();
  Ray ray =this->GetPhotonRay(t);

  double min_t =DBL_MAX-100.0;
  bool haveNode=false;
  int select=-1;
  for(int i=0; i<children.size(); i++){
    double tmp_t =DBL_MAX;
    haveNode =haveNode || children[i].lock()->GetForwardNodeRayParameter(ray, tmp_t);
    if(tmp_t < min_t){
      min_t =tmp_t;
      select=i;
    }
  }

  if(select ==-1){
    return false;
  }

  out =children[select];
  return true;
}



void FlyingPhoton::Update(UniformRandomGenerator0to1 &random)
{
  double tfree =this->CalcFreePath_cm(this->CurrentNode.lock()->GetCompound(), random);
  
  if(this->GetTransportResult(tfree) == TRAVERSE_INTO_CHILD_NODE){
    //std::cout << "child trap" << std::endl;
    WeakPhantomNodePtr nextChild;
    assert( this->GetNextChildNode(tfree, nextChild) );
    this->SetCurrentPhantomNode( nextChild );

    double t=0.0;
    nextChild.lock()->GetForwardNodeRayParameter( this->GetPhotonRay(tfree), t);
    //std::cout << "t:" << t << std::endl;
    this->MoveForwardByRayParameter(t);
    this->SetCurrentPhantomNode(nextChild);
    return;
  }


  if(this->GetTransportResult(tfree) == INTERACTION_INSIDE_NODE){
    //std::cout << "in trap" << std::endl;
    this->MoveForwardByRayParameter(tfree);
    this->DoInteraction( random );
    return;
  }


  if(this->GetTransportResult(tfree) == GET_OUTSIDE_OF_NODE){
    //std::cout << "out trap" << std::endl;
    if( this->CurrentNode.lock()->GetParentName() == "Root" ){
      this->DoesFlyAwayFlag =true;
      return;
    }
    double t=0.0;
    this->CurrentNode.lock()->GetForwardNodeRayParameter( this->GetPhotonRay(tfree), t);
    //std::cout << "t:" << t << std::endl;
    this->MoveForwardByRayParameter(t);
    this->SetCurrentPhantomNode( CurrentNode.lock()->GetParent() );
    return;
  }

}



//////////////////////////////
// Wrap Interaction Sequence
//////////////////////////////
void FlyingPhoton::DoInteraction(UniformRandomGenerator0to1 &random)
{
  const int countMax =10000;
  PhotonInteraction interaction =DetermineInteraction(this->CurrentNode.lock()->GetCompound(), random);
  int count =0;

  switch(interaction){

  case PHOTO_ELECTRIC:
    while( this->PhotoElectric( this->CurrentNode.lock()->GetCompound(), random ) == MC_REJECTED ){
      count++;
      if(count > countMax){
	DoesFlyAwayFlag=true;
	IsAbsorbedFlag=true;
	//std::cerr << "Too many trial @PhotoElectric" << std::endl;
	return;
      }
    }
    break;

  case COHERENT_SCATTERING:
    while( this->CoherentScattering( this->CurrentNode.lock()->GetCompound(), random ) == MC_REJECTED ){
      count++;
      if(count > countMax){
	DoesFlyAwayFlag=true;
	IsAbsorbedFlag=true;
	//std::cerr << "Too many trial @CoherentScattering" << std::endl;
	return;
      }
    }
    break;

  case INCOHERENT_SCATTERING:
    while( this->IncoherentScattering( this->CurrentNode.lock()->GetCompound(), random ) == MC_REJECTED ){
      count++;
      if(count > countMax){
	DoesFlyAwayFlag=true;
	IsAbsorbedFlag=true;
	//std::cerr << "Too many trial @IncoherentScattering" << std::endl;
	return;
      }
    }
    break;


  default:
    assert(!"Interaction Error");
    break;
  }

}
