#ifndef DataFormats_PatCandidates_TriggerPrimitive_h
#define DataFormats_PatCandidates_TriggerPrimitive_h


//
// $Id$
//


/**
  \class    pat::TriggerPrimitive TriggerPrimitive.h "DataFormats/PatCandidates/interface/TriggerPrimitive.h"
  \brief    Analysis-level trigger primitive class

   TriggerPrimitive implements a container for trigger primitives' information within the 'pat' namespace.
   It inherits from LeafCandidate and adds trigger and filter names.

  \author   Volker Adler
  \version  $Id$
*/


#include <string>
#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include "DataFormats/Common/interface/OwnVector.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/Association.h"


namespace pat {

  class TriggerPrimitive : public reco::LeafCandidate {

    public:

      TriggerPrimitive();
      TriggerPrimitive( const pat::TriggerPrimitive & aTrigPrim );
      TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aFilt = "", const int aType = 0 );
      TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aFilt = "", const int aType = 0 );
      virtual ~TriggerPrimitive();
      
      virtual TriggerPrimitive * clone() const;
      
      const std::string & filterName() const;
      const int & triggerObjectType() const;
      
      void setFilterName( const std::string aFilt = "" );
      void setTriggerObjectType( const int aType = 0 );
      
    protected:
    
      std::string filterName_;
      int         triggerObjectType_;

  };
  

  /// collection of TriggerPrimitive
  typedef edm::OwnVector<TriggerPrimitive> TriggerPrimitiveCollection;
  /// persistent reference to a TriggerPrimitive
  typedef edm::Ref<TriggerPrimitiveCollection> TriggerPrimitiveRef;
  /// persistent reference to a TriggerPrimitiveCollection
  typedef edm::RefProd<TriggerPrimitiveCollection> TriggerPrimitiveRefProd;
  /// vector of reference to TriggerPrimitive in the same collection
  typedef edm::RefVector<TriggerPrimitiveCollection> TriggerPrimitiveRefVector;
  /// vector of reference to TriggerPrimitive in the same collection
  typedef edm::Association<TriggerPrimitiveCollection> TriggerPrimitiveMatch;

}


#endif
