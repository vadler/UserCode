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

#include <vector>


namespace pat {

  class TriggerPrimitive : public reco::LeafCandidate {

    public:

      TriggerPrimitive();
      TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aTrig = "", const std::string aFilt = "" );
      TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aTrig = "", const std::string aFilt = "" );
      virtual ~TriggerPrimitive();
      
      const std::string & triggerName() const;
      const std::string & filterName() const;
      
      void setTriggerName( const std::string aTrig = "" );
      void setFilterName( const std::string aFilt = "" );
      
    protected:
    
      std::string triggerName_;
      std::string filterName_;      

  };
  
  typedef std::vector<TriggerPrimitive> TriggerPrimitiveCollection;

}


#endif
