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

#include <string>
#include <vector>

#include "DataFormats/HLTReco/interface/TriggerObject.h"
// #include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
// #include "DataFormats/Common/interface/Ptr.h"
// #include "DataFormats/Common/interface/PtrVector.h"


namespace pat {

  class TriggerObject : public reco::LeafCandidate {

    public:

      /// constructors and desctructor
      TriggerObject();
      TriggerObject( const reco::Particle::LorentzVector & vec, const int id = 0 );
      TriggerObject( const reco::Particle::PolarLorentzVector & vec, const int id = 0 );
      TriggerObject( const trigger::TriggerObject & aTrigObj );
      virtual ~TriggerObject();
      
      /// setters & getters
      void setCollection( std::string & collection );
      void addFilterId( unsigned int filterId );
      std::string                 collection() const;
      std::vector< unsigned int > filterIds() const;
      unsigned int                filterId( const unsigned int i ) const;
      unsigned int                sizeFilterIds() const;
      bool                        hasFilterId( const unsigned int filterId ) const;                 
       
    protected:
    
      /// data members
      std::string                 collection_;
      std::vector< unsigned int > filterIds_;  // special filter related ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
    
//       std::vector< edm::RefToBase< reco::Candidate > > patObjectMatches_; // initialization?

  };
  

  /// collection of TriggerObject
  typedef std::vector< TriggerObject >              TriggerObjectCollection;
  /// persistent reference to an item in a TriggerObjectCollection
  typedef edm::Ref< TriggerObjectCollection >       TriggerObjectRef;
  /// persistent reference to a TriggerObjectCollection product
  typedef edm::RefProd< TriggerObjectCollection >   TriggerObjectRefProd;
  /// vector of persistent references to items in the same TriggerObjectCollection
  typedef edm::RefVector< TriggerObjectCollection > TriggerObjectRefVector;
//   /// persistent reference to an item in a TriggerObjectCollection in the edm::Event
//   typedef edm::Ptr< TriggerObjectCollection >       TriggerObjectPtr;
//   /// vector of persistent references to items in the same TriggerObjectCollection in the edm::Event
//   typedef edm::PtrVector< TriggerObjectCollection > TriggerObjectPtrVector;

}


#endif
