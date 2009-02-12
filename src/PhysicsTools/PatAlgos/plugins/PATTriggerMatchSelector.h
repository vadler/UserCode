#ifndef PhysicsTools_PatAlgos_PATTriggerMatchSelector_h
#define PhysicsTools_PatAlgos_PATTriggerMatchSelector_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      PATTriggerMatchSelector
//
/**
  \class    pat::PATTriggerMatchSelector PATTriggerMatchSelector.h "PhysicsTools/PatAlgos/plugins/PATTriggerMatchSelector.h"
  \brief    Dummy class in order to use PATCandMatcher

   Dummy class.

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
    
      std::string                 triggerName_; // path name or filter label
      std::vector< unsigned int > filterIds_;   // special filter related ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
  
    public:
    
      PATTriggerMatchSelector( const edm::ParameterSet & iConfig ) :
        triggerName_( iConfig.getParameter< std::string >( "triggerName" ) ),
        filterIds_( iConfig.getParameter< std::vector< unsigned int > >( "filterIds" ) )
      { }
      
      bool operator()( const T1 & patObj, const T2 & trigObj ) const {
        // no restriction to trigger name, which triggers check for filter ID
        if ( triggerName_ == "@" || triggerName_ == "" ) {
          // match only for given filter ID (0 --> everything)
          for ( unsigned int i = 0; i < filterIds_.size(); ++i ) {
            if ( filterIds_.at( i ) == 0 || trigObj.hasFilterId( filterIds_.at( i ) ) ) return true;
          }
          return false;
        }
        // match only objects used in given trigger path/filter
        if ( true ) { // FIXME develop correct path/filter selector
          return true;
        }
        return false;
      }
      
  };
  
}


#endif
