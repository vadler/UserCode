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


// #include "DataFormats/PatCandidates/interface/TriggerObject.h"
// #include "DataFormats/PatCandidates/interface/TriggerPath.h"
// #include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include <string>
#include <vector>
   
#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

namespace pat {

  class TriggerFilter {

    public:

      /// constructors and desctructor
      TriggerFilter();
      TriggerFilter( std::string & label, std::string & type );
      TriggerFilter( edm::InputTag & tag, std::string & type );
      virtual ~TriggerFilter();
      
      /// filter related
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
      
      /// objects related
/*      pat::TriggerObjectCollection getObjects();
      unsigned int                 nObjects();*/
      
      /// paths related
      
      /// event related
      
    protected:
    
      std::string                 label_;
      std::string                 type_;
      std::string                 labelInput_;
      unsigned int                objectId_; // what is this in this case?
      bool                        run_;
      bool                        accept_;
//       pat::TriggerObjectRefVector objects_; // initialization?
        
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
