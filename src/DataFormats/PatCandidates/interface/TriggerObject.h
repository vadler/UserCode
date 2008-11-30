#ifndef DataFormats_PatCandidates_TriggerObject_h
#define DataFormats_PatCandidates_TriggerObject_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerObject
//
// $Id$
//
/**
  \class    pat::TriggerObject TriggerObject.h "DataFormats/PatCandidates/interface/TriggerObject.h"
  \brief    Analysis-level trigger object class

   TriggerObject implements a container for trigger objects' information within the 'pat' namespace.
   It inherits from LeafCandidate and adds the following data members:
   - [to be filled]
   In addition, the data member reco::Particle::pdgId_ (inherited via reco::LeafCandidate) is used
   to store the trigger object id from trigger::TriggerObject::id_.

  \author   Volker Adler
  \version  $Id$
*/


#include "DataFormats/Candidate/interface/LeafCandidate.h"

// #include "DataFormats/PatCandidates/interface/TriggerFilter.h"
// #include "DataFormats/PatCandidates/interface/TriggerPath.h"
// #include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include <vector>

#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"


namespace pat {

  class TriggerObject : public reco::LeafCandidate {

    public:

      /// constructors and desctructor
      TriggerObject();
      TriggerObject( const reco::Particle::LorentzVector & vec, const int id = 0 );
      TriggerObject( const reco::Particle::PolarLorentzVector & vec, const int id = 0 );
      TriggerObject( const trigger::TriggerObject & aTrigObj );
      virtual ~TriggerObject();
      
      /// object related
      
      /// filters related
      
      /// PATObject match related
      
    protected:
    
      std::vector<edm::RefToBase<reco::Candidate> > patObjectMatches_; // initialization?

  };
  

  /// collection of TriggerObject
  typedef std::vector< TriggerObject >                TriggerObjectCollection;
  /// persistent reference to a TriggerObjectCollection
  typedef edm::Ref< TriggerObjectCollection >         TriggerObjectRef;
  /// persistent reference to a TriggerObjectCollection product
  typedef edm::RefProd< TriggerObjectCollection >     TriggerObjectRefProd;
  /// vector of reference to TriggerObject in the same collection
  typedef edm::RefVector< TriggerObjectCollection >   TriggerObjectRefVector;

}


#endif
