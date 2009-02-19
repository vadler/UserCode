#ifndef DataFormats_PatCandidates_TriggerObject_h
#define DataFormats_PatCandidates_TriggerObject_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerObject
//
// $Id: TriggerObject.h,v 1.1.2.1 2008/12/18 11:26:15 vadler Exp $
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
  \version  $Id: TriggerObject.h,v 1.1.2.1 2008/12/18 11:26:15 vadler Exp $
*/


#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include <map>
#include <string>
#include <vector>

#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/Association.h"


namespace pat {

  class TriggerObject : public reco::LeafCandidate {

    public:

      /// constructors and desctructor
      TriggerObject();
      TriggerObject( const reco::Particle::LorentzVector & vec, int id = 0 );
      TriggerObject( const reco::Particle::PolarLorentzVector & vec, int id = 0 );
      TriggerObject( const trigger::TriggerObject & aTrigObj );
      virtual ~TriggerObject();
      
      /// setters & getters
      void setCollection( const std::string & collection );
      void addFilterId( unsigned filterId );
      std::string             collection() const;
      std::vector< unsigned > filterIds() const;
      bool                    hasFilterId( unsigned filterId ) const;
       
    protected:
    
      /// data members
      std::string             collection_;
      std::vector< unsigned > filterIds_;  // special filter related ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
                                           // empty, if object was not used in last active filter

  };
  

  /// collection of TriggerObject
  typedef std::vector< TriggerObject >                  TriggerObjectCollection;
  /// persistent reference to an item in a TriggerObjectCollection
  typedef edm::Ref< TriggerObjectCollection >           TriggerObjectRef;
  /// persistent reference to a TriggerObjectCollection product
  typedef edm::RefProd< TriggerObjectCollection >       TriggerObjectRefProd;
  /// vector of persistent references to items in the same TriggerObjectCollection
  typedef edm::RefVector< TriggerObjectCollection >     TriggerObjectRefVector;
  /// association of TriggerObjects to store matches to Candidates
  typedef edm::Association< TriggerObjectCollection >   TriggerObjectMatches;
  /// container to store matches from different producers
  typedef std::map< std::string, TriggerObjectMatches > TriggerObjectMatchesCollection;

}


#endif
