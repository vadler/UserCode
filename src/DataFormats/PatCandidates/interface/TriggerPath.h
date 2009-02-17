#ifndef DataFormats_PatCandidates_TriggerPath_h
#define DataFormats_PatCandidates_TriggerPath_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerPath
//
// $Id: TriggerPath.h,v 1.1.2.1 2008/12/18 11:26:15 vadler Exp $
//
/**
  \class    pat::TriggerPath TriggerPath.h "DataFormats/PatCandidates/interface/TriggerPath.h"
  \brief    Analysis-level trigger object class

   TriggerPath implements a container for trigger paths' information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id: TriggerPath.h,v 1.1.2.1 2008/12/18 11:26:15 vadler Exp $
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
      TriggerPath( const std::string & name);
      TriggerPath( const std::string & name, unsigned prescale, bool run, bool accept, bool error, unsigned lastActiveModule );
      virtual ~TriggerPath();
      
      /// setters & getters
      void setName( const std::string & name );
      void setPrescale( unsigned prescale );
      void setRun( bool run );
      void setAccept( bool accept );
      void setError( bool error );
      void addModule( const std::string & name );
      void setLastActiveModule( unsigned lastActiveModule );
      std::string                name() const;
      unsigned                   prescale() const;
      bool                       wasRun() const;
      bool                       wasAccept() const;
      bool                       wasError() const;
      std::vector< std::string > modules() const; // ordered
      unsigned                   indexModule( const std::string & name ) const; // returns sizeModules() if name unknown
      unsigned                   lastActiveModule() const;
      
    protected:
    
      /// data members
      std::string                name_;
      unsigned                   prescale_;
      bool                       run_;
      bool                       accept_;
      bool                       error_;
      std::vector< std::string > modules_; // ordered
      unsigned                   lastActiveModule_;
    
  };
  

  /// collection of TriggerPath
  typedef std::vector< TriggerPath >              TriggerPathCollection;
  /// persistent reference to an item in a TriggerPathCollection
  typedef edm::Ref< TriggerPathCollection >       TriggerPathRef;
  /// persistent reference to a TriggerPathCollection product
  typedef edm::RefProd< TriggerPathCollection >   TriggerPathRefProd;
  /// vector of persistent references to items in the same TriggerPathCollection
  typedef edm::RefVector< TriggerPathCollection > TriggerPathRefVector;

}


#endif
