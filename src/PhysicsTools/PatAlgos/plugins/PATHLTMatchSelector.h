#ifndef PhysicsTools_PatAlgos_PATHLTMatchSelector_h
#define PhysicsTools_PatAlgos_PATHLTMatchSelector_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      PATHLTMatchSelector
//
/**
  \class    pat::PATHLTMatchSelector PATHLTMatchSelector.h "PhysicsTools/PatAlgos/interface/PATHLTMatchSelector.h"
  \brief    Dummy class as counterpart to PATMatchSelector in order to use PATCandMatcher

   Dummy class.
   This might be replaced later by checks if the particular PATObject could have been fired the requested trigger/filter at all.

  \author   Volker Adler
  \version  $Id$
*/
//
// $Id$
//


#include "FWCore/ParameterSet/interface/ParameterSet.h"


namespace pat {

  template<typename T1, typename T2>
  class PATHLTMatchSelector {
  
    public:
    
      PATHLTMatchSelector( const edm::ParameterSet& cfg ) {  }
      
      bool operator()( const T1 & c, const T2 & hlt ) const { return true; }
      
    private:
      
  };
  
}


#endif
