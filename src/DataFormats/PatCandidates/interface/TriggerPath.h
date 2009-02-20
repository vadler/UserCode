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
    
      /// data members
      std::string                name_;
      unsigned                   prescale_;
      bool                       run_;
      bool                       accept_;
      bool                       error_;
      std::vector< std::string > modules_; // ordered
      unsigned                   lastActiveModule_;

    public:

      /// constructors and desctructor
      TriggerPath();
      TriggerPath( const std::string & name);
      TriggerPath( const std::string & name, unsigned prescale, bool run, bool accept, bool error, unsigned lastActiveModule );
      virtual ~TriggerPath() {};
      
      /// setters & getters
      void setName( const std::string & name )              { name_ = name; };
      void setPrescale( unsigned prescale )                 { prescale_ = prescale; };
      void setRun( bool run )                               { run_ = run; };
      void setAccept( bool accept )                         { accept_ = accept; };
      void setError( bool error )                           { error_ = error; };
      void addModule( const std::string & name )            { modules_.push_back( name ); };
      void setLastActiveModule( unsigned lastActiveModule ) { lastActiveModule_ = lastActiveModule; };
      std::string                name() const             { return name_; };
      unsigned                   prescale() const         { return prescale_; };
      bool                       wasRun() const           { return run_; };
      bool                       wasAccept() const        { return accept_; };
      bool                       wasError() const         { return error_; };
      std::vector< std::string > modules() const          { return modules_; }; // ordered
      unsigned                   lastActiveModule() const { return lastActiveModule_; };
      unsigned                   indexModule( const std::string & name ) const; // returns size of modules_ if name unknown
    
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
