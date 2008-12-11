#ifndef DataFormats_PatCandidates_TriggerEvent_h
#define DataFormats_PatCandidates_TriggerEvent_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerEvent
//
// $Id$
//
/**
  \class    pat::TriggerEvent TriggerEvent.h "DataFormats/PatCandidates/interface/TriggerEvent.h"
  \brief    Analysis-level trigger object class

   TriggerEvent implements a container for trigger event's information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id$
*/


#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"

#include <string>

#include "DataFormats/Provenance/interface/ProductID.h"
// #include "DataFormats/Common/interface/AssociationMap.h"
   

namespace pat {

  class TriggerEvent {

    public:

      /// constructors and desctructor
      TriggerEvent();
      TriggerEvent( std::string & nameHltTable, bool run, bool accept, bool error );
      virtual ~TriggerEvent();
      
      /// event related
      void setNameHltTable( std::string & name );
      void setRun( bool run );
      void setAccept( bool accept );
      void setError( bool error );
      std::string nameHltTable() const;
      bool        wasRun() const;
      bool        wasAccept() const;
      bool        wasError() const;
      
      /// paths related
      
      /// filters related
      
      /// objects related
      
    protected:
    
      /// event related data members
      std::string nameHltTable_;
      bool        run_;
      bool        accept_;
      bool        error_;
      
      /// paths related data members
      TriggerPathRefProd paths_;
      
      /// filters related data members
      TriggerFilterRefProd filters_;
      
      /// objects related data members
      TriggerObjectRefProd objects_;
        
  };

}


#endif
