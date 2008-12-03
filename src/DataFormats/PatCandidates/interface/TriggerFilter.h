#ifndef DataFormats_PatCandidates_TriggerFilter_h
#define DataFormats_PatCandidates_TriggerFilter_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerFilter
//
// $Id$
//
/**
  \class    pat::TriggerFilter TriggerFilter.h "DataFormats/PatCandidates/interface/TriggerFilter.h"
  \brief    Analysis-level trigger object class

   TriggerFilter implements a container for trigger filters' information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id$
*/


#include <string>
#include <vector>
   
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"

namespace pat {

  class TriggerFilter {

    public:

      /// constructors and desctructor
      TriggerFilter();
      TriggerFilter( const std::string & label );
      TriggerFilter( const edm::InputTag & tag );
      virtual ~TriggerFilter();
      
      /// setters & getters
      void setLabel( std::string & label );
      void setType( std::string & type );
      void setLableInput( std::string & labelInput );
      void setObjectId( unsigned int objectId );
      void setWasRun( bool run );
      void setWasAccept( bool accept );
      std::string & getLabel();
      std::string & getType();
      std::string & getLabelInput();
      unsigned int  getObjectId();                 
      bool          wasRun();
      bool          wasAccept();
      
    protected:
    
      std::string  label_;
      std::string  type_;
      std::string  labelInput_;
      unsigned int objectId_; // what is this in this case (alternating sometimes, e.g.: 84, 86)?
      bool         run_;
      bool         accept_;
        
  };
  

  /// collection of TriggerFilter
  typedef std::vector< TriggerFilter >              TriggerFilterCollection;
  /// persistent reference to a TriggerFilterCollection
  typedef edm::Ref< TriggerFilterCollection >       TriggerFilterRef;
  /// persistent reference to a TriggerFilterCollection product
  typedef edm::RefProd< TriggerFilterCollection >   TriggerFilterRefProd;
  /// vector of reference to TriggerFilter in the same collection
  typedef edm::RefVector< TriggerFilterCollection > TriggerFilterRefVector;

}


#endif
