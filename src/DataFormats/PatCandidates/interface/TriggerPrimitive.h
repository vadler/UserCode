//
// $Id$
//


#ifndef DataFormats_PatCandidates_TriggerPrimitive_h
#define DataFormats_PatCandidates_TriggerPrimitive_h


#include <string>
#include "DataFormats/Candidate/interface/Particle.h"

#include <vector>


/**
  \class    pat::TriggerPrimitive TriggerPrimitive.h "DataFormats/PatCandidates/interface/TriggerPrimitive.h"
  \brief    Analysis-level trigger primitive class

   TriggerPrimitive implements a container for trigger primitives' information within the 'pat' namespace.

  \author   Volker Adler
  \version  $Id$
*/


namespace pat {

  class TriggerPrimitive {

    public:

      TriggerPrimitive();
      TriggerPrimitive( const reco::Particle::LorentzVector & aVec, const std::string aTrig = "", const std::string aFilt = "" );
//       TriggerPrimitive( const reco::Particle::PolarLorentzVector & aVec, const std::string aTrig = "", const std::string aFilt = "" );
      TriggerPrimitive( const TriggerPrimitive & aPrim );
      virtual ~TriggerPrimitive();
      
      const reco::Particle::LorentzVector & lorentzVector() const;
      const std::string                   & triggerName() const;
      const std::string                   & filterName() const;
      
      void setLorentzVector( const reco::Particle::LorentzVector & aVec );
//       void setLorentzVector( const reco::Particle::PolarLorentzVector & aVec );
      void setTriggerName( const std::string aTrig = "" );
      void setFilterName( const std::string aFilt = "" );
      
    protected:
    
      reco::Particle::LorentzVector lorentzVector_;
      std::string                   triggerName_;
      std::string                   filterName_;      

  };
  
  typedef std::vector<TriggerPrimitive> TriggerPrimitiveCollection;

}


#endif
