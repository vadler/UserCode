#ifndef PhysicsTools_PatAlgos_PATHLTMatchSelector_h
#define PhysicsTools_PatAlgos_PATHLTMatchSelector_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      PATHLTMatchSelector
//
/**
  \class    PATHLTMatchSelector PATHLTMatchSelector.h "PhysicsTools/PatAlgos/interface/PATHLTMatchSelector.h"
  \brief    

   Dummy class.
   This might be replaced later by checks if the particular PATObject cold have been fired the requested trigger/filter at all.

  \author   Volker Adler
  \version  $Id: PATHLTMatchSelector.h,v 1.1 2008/02/22 11:27:45 vadler Exp $
*/
//
// $Id: PATHLTMatchSelector.h,v 1.1 2008/02/22 11:27:45 vadler Exp $
//


#include "FWCore/ParameterSet/interface/ParameterSet.h"


namespace pat {

  template<typename T1, typename T2>
  class PATHLTMatchSelector {
  
    public:
    
      PATHLTMatchSelector(const edm::ParameterSet& cfg) {  }
      
      bool operator()( const T1 & c, const T2 & hlt ) const { return true; }
      
    private:
      
  };
  
}


#endif
