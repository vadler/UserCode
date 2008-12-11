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
// #include "DataFormats/Common/interface/Ptr.h"
// #include "DataFormats/Common/interface/PtrVector.h"


namespace pat {

  class TriggerPath {

    public:

      /// constructors and desctructor
      TriggerPath();
      TriggerPath( std::string & name);
      TriggerPath( std::string & name, unsigned int prescale, bool run, bool accept, bool error, unsigned int lastModule );
      virtual ~TriggerPath();
      
      /// setters & getters
      void setName( std::string & name );
      void setPrescale( unsigned int prescale );
      void setRun( bool run );
      void setAccept( bool accept );
      void setError( bool error );
      void setLastModule( unsigned int lastModule );
      std::string  name() const;
      unsigned int prescale() const;
      bool         wasRun() const;
      bool         wasAccept() const;
      bool         wasError() const;
      unsigned int lastModule() const;
      
    protected:
    
      /// data members
      std::string  name_;
      unsigned int prescale_;
      bool         run_;
      bool         accept_;
      bool         error_;
      unsigned int lastModule_;
    
  };
  

  /// collection of TriggerPath
  typedef std::vector< TriggerPath >              TriggerPathCollection;
  /// persistent reference to an item in a TriggerPathCollection
  typedef edm::Ref< TriggerPathCollection >       TriggerPathRef;
  /// persistent reference to a TriggerPathCollection product
  typedef edm::RefProd< TriggerPathCollection >   TriggerPathRefProd;
  /// vector of persistent references to items in the same TriggerPathCollection
  typedef edm::RefVector< TriggerPathCollection > TriggerPathRefVector;
//   /// persistent reference to an item in a TriggerPathCollection in the edm::Event
//   typedef edm::Ptr< TriggerPathCollection >       TriggerPathPtr;
//   /// vector of persistent references to items in the same TriggerPathCollection in the edm::Event
//   typedef edm::PtrVector< TriggerPathCollection > TriggerPathPtrVector;

}


#endif
