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
    
      std::vector< unsigned >    filterIds_;    // special filter related ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
      std::vector< std::string > filterLabels_;
      std::vector< std::string > pathNames_;
  
    public:
    
      PATTriggerMatchSelector( const edm::ParameterSet & iConfig ) :
        filterIds_( iConfig.getParameter< std::vector< unsigned > >( "filterIds" ) ),
        filterLabels_( iConfig.getParameter< std::vector< std::string > >( "filterLabels" ) ),
        pathNames_( iConfig.getParameter< std::vector< std::string > >( "pathNames" ) )
      {
      }
      
      bool operator()( const T1 & patObj, const T2 & trigObj ) const {
        for ( unsigned i = 0; i < filterIds_.size(); ++i ) {
          if ( filterIds_.at( i ) == 0 || trigObj.hasFilterId( filterIds_.at( i ) ) ) {
            for ( unsigned j = 0; j < filterLabels_.size(); ++j ) {
              if ( filterLabels_.at( j ) == "*" || filterLabels_.at( j ) == "@" || trigObj.hasFilterLabel( filterLabels_.at( j ) ) ) {
                for ( unsigned k = 0; k < filterLabels_.size(); ++k ) {
                  if ( pathNames_.at( k ) == "*" || pathNames_.at( k ) == "@" || trigObj.hasPathName( pathNames_.at( k ) ) ) {
                    return true;
                  }
                }
              }
            }
          }
        }
        return false;
      }
      
  };
  
}


#endif
