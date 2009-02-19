#ifndef DataFormats_PatCandidates_TriggerEvent_h
#define DataFormats_PatCandidates_TriggerEvent_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerEvent
//
// $Id: TriggerEvent.h,v 1.1.2.1 2008/12/18 11:26:14 vadler Exp $
//
/**
  \class    pat::TriggerEvent TriggerEvent.h "DataFormats/PatCandidates/interface/TriggerEvent.h"
  \brief    Analysis-level trigger object class

   TriggerEvent implements a container for trigger event's information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id: TriggerEvent.h,v 1.1.2.1 2008/12/18 11:26:14 vadler Exp $
*/


#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"

#include <string>
#include <vector>

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
   

namespace pat {

  class TriggerEvent {
      
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
      TriggerObjectRefProd  objects_;
      TriggerObjectMatchMap objectMatchResults_;

    public:
    
      /// constructors and desctructor
      TriggerEvent() { objectMatchResults_.clear(); };
      TriggerEvent( const std::string & nameHltTable, bool run, bool accept, bool error );
      virtual ~TriggerEvent() {};
      
      /// event related
      void setNameHltTable( const std::string & name ) { nameHltTable_ = name; };
      void setRun( bool run )                          { run_          = run; };
      void setAccept( bool accept )                    { accept_       = accept; };
      void setError( bool error )                      { error         = error; };
      std::string nameHltTable() const                 { return nameHltTable_; };
      bool        wasRun() const                       { return run_; };
      bool        wasAccept() const                    { return accept_; };
      bool        wasError() const                     { return error_; };
      
      /// paths related
      void setPaths( const edm::Handle< TriggerPathCollection > & handleTriggerPaths );
      const TriggerPathCollection * paths() const;                                   // returns 0 if RefProd is null
      const TriggerPath           * path( const std::string & namePath ) const;      // returns 0 if path is not found
      unsigned                      indexPath( const std::string & namePath ) const; // returns size of filter collection if filter is not found
      TriggerPathRefVector          acceptedPaths() const;                           // transient
      
      /// filters related
      void setFilters( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters );
      const TriggerFilterCollection * filters() const;                                      // returns 0 if RefProd is null
      const TriggerFilter           * filter( const std::string & labelFilter ) const;      // returns 0 if filter is not found
      unsigned                        indexFilter( const std::string & labelFilter ) const; // returns size of filter collection if filter is not found
      TriggerFilterRefVector          acceptedFilters() const;                              // transient
      
      /// objects related
      void setObjects( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects );
      bool addObjectMatchResult( const TriggerObjectMatch & trigMatches, const std::string & labelMatcher ); // returns 'false' if 'matcher' alreadey exists
      const TriggerObjectCollection * objects() const;                                             // returns 0 if RefProd is null
      TriggerObjectRefVector          objects( unsigned filterId ) const;                          // transient
      const TriggerObjectMatchMap   * objectMatchResults() const;
      const TriggerObjectMatch      * objectMatchResult( const std::string & labelMatcher ) const; // returns 0 if 'labelMatcher' not found
      
      /// x-collection related
      TriggerFilterRefVector     pathFilters( const std::string & namePath, bool all = true ) const;                                      // transient; setting 'all' to 'false' returns the run filters only.
      bool                       filterInPath( const TriggerFilterRef & filterRef, const std::string & namePath, bool all = true ) const; // setting 'all' to 'false' returns false, if the filter was not run though in path.
      TriggerPathRefVector       filterPaths( const TriggerFilterRef & filterRef ) const;                                                 // transient
      std::vector< std::string > filterCollections( const std::string & labelFilter ) const;                                              // returns the used collections, not the configuration
      TriggerObjectRefVector     filterObjects( const std::string & labelFilter ) const;                                                  // transient
      bool                       objectInFilter( const TriggerObjectRef & objectRef, const std::string & labelFilter ) const;
      TriggerFilterRefVector     objectFilters( const TriggerObjectRef & objectRef ) const;                                               // transient
      TriggerObjectRefVector     pathObjects( const std::string & namePath ) const;                                                       // transient
      bool                       objectInPath( const TriggerObjectRef & objectRef, const std::string & namePath ) const;
      TriggerPathRefVector       objectPaths( const TriggerObjectRef & objectRef  ) const;                                                // transient
      
      /// trigger matches
      TriggerObjectRefVector      triggerMatchObjects( const reco::CandidateBaseRef & candRef, const std::string & labelMatcher ) const; // transient
      TriggerObjectRefVector      triggerMatchObjectsFilter( const reco::CandidateBaseRef & candRef, const std::string & labelMatcher, const std::string & labelFilter ) const; // transient
      TriggerObjectRefVector      triggerMatchObjectsPath( const reco::CandidateBaseRef & candRef, const std::string & labelMatcher, const std::string & namePath ) const; // transient
      TriggerObjectMatchContainer triggerMatchObjectsAll( const reco::CandidateBaseRef & candRef ) const; // transient
      TriggerObjectMatchContainer triggerMatchObjectsFilterAll( const reco::CandidateBaseRef & candRef, const std::string & labelFilter ) const; // transient
      TriggerObjectMatchContainer triggerMatchObjectsPathAll( const reco::CandidateBaseRef & candRef, const std::string & namePath ) const; // transient
      
  };

}


#endif
