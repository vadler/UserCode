#ifndef PhysicsTools_PatAlgos_PATTriggerMatchSelector_h
#define PhysicsTools_PatAlgos_PATTriggerMatchSelector_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      PATTriggerMatchSelector
//
/**
  \class    pat::PATTriggerMatchSelector PATTriggerMatchSelector.h "PhysicsTools/PatAlgos/plugins/PATTriggerMatchSelector.h"
  \brief    

   .

  \author   Volker Adler
  \version  $Id$
*/
//
// $Id$
//


#include <string>
#include <vector>

#include "FWCore/ParameterSet/interface/ParameterSet.h"


namespace pat {

  template< typename T1, typename T2 >
  class PATTriggerMatchSelector {
    
      std::vector< unsigned > filterIds_;   // special filter related ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
  
    public:
    
      PATTriggerMatchSelector( const edm::ParameterSet & iConfig ) :
        filterIds_( iConfig.getParameter< std::vector< unsigned > >( "filterIds" ) )
      {
      }
      
      bool operator()( const T1 & patObj, const T2 & trigObj ) const {
        for ( unsigned i = 0; i < filterIds_.size(); ++i ) {
          if ( filterIds_.at( i ) == 0 || trigObj.hasFilterId( filterIds_.at( i ) ) )
            return true;
        }
        return false;
      }
      
  };
  
}


#endif
