#ifndef DataFormats_PatCandidates_TriggerEvent_h
#define DataFormats_PatCandidates_TriggerEvent_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerEvent
//
// $Id: TriggerEvent.h,v 1.1.2.5 2008/12/16 18:01:57 vadler Exp $
//
/**
  \class    pat::TriggerEvent TriggerEvent.h "DataFormats/PatCandidates/interface/TriggerEvent.h"
  \brief    Analysis-level trigger object class

   TriggerEvent implements a container for trigger event's information within the 'pat' namespace.
   It has the following data members:
   - [to be filled]

  \author   Volker Adler
  \version  $Id: TriggerEvent.h,v 1.1.2.5 2008/12/16 18:01:57 vadler Exp $
*/


#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerObject.h"

#include <string>
#include <vector>

#include "DataFormats/Common/interface/Handle.h"
   

namespace pat {

  class TriggerEvent {

    public:

      /// constructors and desctructor
      TriggerEvent();
      TriggerEvent( const std::string & nameHltTable, bool run, bool accept, bool error );
      virtual ~TriggerEvent();
      
      /// event related
      void setNameHltTable( const std::string & name );
      void setRun( bool run );
      void setAccept( bool accept );
      void setError( bool error );
      std::string nameHltTable() const;
      bool        wasRun() const;
      bool        wasAccept() const;
      bool        wasError() const;
      
      /// paths related
      void setPathCollection( const edm::Handle< TriggerPathCollection > & handleTriggerPaths );
      void setPaths( const edm::Handle< TriggerPathCollection > & handleTriggerPaths );
      const TriggerPathCollection * pathCollection() const;                          // returns 0 if RefProd is null
      const TriggerPathCollection * paths() const;                                   // returns 0 if RefProd is null
      const TriggerPath           * path( const std::string & namePath ) const;      // returns 0 if path is not found
      unsigned int                  indexPath( const std::string & namePath ) const; // returns size of filter collection if filter is not found
      TriggerPathRefVector          acceptedPaths() const;                           // transient
      
      /// filters related
      void setFilterCollection( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters );
      void setFilters( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters );
      const TriggerFilterCollection * filterCollection() const;                             // returns 0 if RefProd is null
      const TriggerFilterCollection * filters() const;                                      // returns 0 if RefProd is null
      const TriggerFilter           * filter( const std::string & labelFilter ) const;      // returns 0 if filter is not found
      unsigned int                    indexFilter( const std::string & labelFilter ) const; // returns size of filter collection if filter is not found
      TriggerFilterRefVector          acceptedFilters() const;                              // transient
      
      /// objects related
      void setObjectCollection( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects );
      void setObjects( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects );
      const TriggerObjectCollection * objectCollection() const; // returns 0 if RefProd is null
      const TriggerObjectCollection * objects() const;          // returns 0 if RefProd is null
      
      /// x-collection related
      TriggerFilterRefVector     pathFilters( const std::string & namePath, bool all = true ) const; // transient; setting 'all' to 'false' returns the run filters only.
      std::vector< std::string > filterCollections( const std::string & labelFilter ) const;         // returns the used collections, not the configuration
      TriggerObjectRefVector     filterObjects( const std::string & labelFilter ) const;             // transient
      TriggerObjectRefVector     pathObjects( const std::string & namePath ) const;                  // transient
      
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
