#ifndef DataFormats_PatCandidates_TriggerPath_h
#define DataFormats_PatCandidates_TriggerPath_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerPath
//
// $Id$
//
/**
  \class    pat::TriggerPath TriggerPath.h "DataFormats/PatCandidates/interface/TriggerPath.h"
  \brief    Analysis-level trigger object class

   TriggerPath implements a container for trigger paths' information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id$
*/


#include <string>
#include <vector>

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"


namespace pat {

  class TriggerPath {

    public:

      /// constructors and desctructor
      TriggerPath();
      TriggerPath( std::string & name);
      TriggerPath( std::string & name, unsigned int prescale, bool run, bool accept, bool error);
      virtual ~TriggerPath();
      
      /// setters & getters
      void setName( std::string & name );
      void setPrescale( unsigned int prescale );
      void setRun( bool run );
      void setAccept( bool accept );
      void setError( bool error );
      std::string  getName();
      unsigned int getPrescale();
      bool         wasRun();
      bool         wasAccept();
      bool         wasError();
      
    protected:
    
      std::string  name_;
      unsigned int prescale_;
      bool         run_;
      bool         accept_;
      bool         error_;
    
  };
  

  /// collection of TriggerPath
  typedef std::vector< TriggerPath >              TriggerPathCollection;
  /// persistent reference to a TriggerPathCollection
  typedef edm::Ref< TriggerPathCollection >       TriggerPathRef;
  /// persistent reference to a TriggerPathCollection product
  typedef edm::RefProd< TriggerPathCollection >   TriggerPathRefProd;
  /// vector of reference to TriggerPath in the same collection
  typedef edm::RefVector< TriggerPathCollection > TriggerPathRefVector;

}


#endif
