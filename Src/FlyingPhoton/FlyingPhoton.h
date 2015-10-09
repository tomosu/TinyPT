#pragma once
#include "PhantomTree.h"
#include "GeometricProperty.h"
#include "Photon.h"
#include "RandomNumberGenerator.h"
#include <tr1/memory>
#include <assert.h>

class PhantomNode;
typedef std::tr1::shared_ptr<PhantomNode> SharedPhantomNodePtr;
typedef std::tr1::weak_ptr<PhantomNode> WeakPhantomNodePtr;

class PhantomTree;
typedef std::tr1::shared_ptr<PhantomTree> SharedPhantomTreePtr;

class FlyingPhoton;
typedef std::tr1::shared_ptr<FlyingPhoton> SharedFlyingPhotonPtr;



enum TransportResult{
  TRAVERSE_INTO_CHILD_NODE=0,
  INTERACTION_INSIDE_NODE=1,
  GET_OUTSIDE_OF_NODE=2,
};


class FlyingPhoton : public Photon
{
 public:
  friend class PhantomTree;
  
  FlyingPhoton(const double energy_MeV, const PointVector_t &direction, const PointVector_t&currentPosition);

  static SharedFlyingPhotonPtr CreateFlyingPhoton(const double energy_MeV,
						  const PointVector_t &direction,
						  const PointVector_t &currentPosition);

  void Update(UniformRandomGenerator0to1 &);

  PointVector_t GetCurrentPosition(){ return this->CurrentPosition;}
  
  bool IsAbsorbed(){
    return this->IsAbsorbedFlag;
  };
  
  bool DoesFlyAway(){ return DoesFlyAwayFlag;}


 private:

  void SetCurrentPhantomNode(const WeakPhantomNodePtr&in){
    CurrentNode=in;
  }

  Ray GetPhotonRay(const double);

  TransportResult GetTransportResult(const double t);

  bool GetNextChildNode(const double, WeakPhantomNodePtr&);

  void MoveForwardByRayParameter(const double t){ SetCurrentPosition( CalcNextPosition(t));}

  void SetCurrentPosition(const PointVector_t &in){ this->CurrentPosition =in;}

  PointVector_t CalcNextPosition(const double t){ return Vadd(this->CurrentPosition, Scalar(this->Direction, t));};

  void DoInteraction(UniformRandomGenerator0to1 &random);

  bool DoesFlyAwayFlag;
  WeakPhantomNodePtr CurrentNode;
  PointVector_t CurrentPosition;
};
