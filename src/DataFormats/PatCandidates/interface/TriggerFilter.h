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
// #include "DataFormats/Common/interface/Ptr.h"
// #include "DataFormats/Common/interface/PtrVector.h"

namespace pat {

  class TriggerFilter {

    public:

      /// constructors and desctructor
      TriggerFilter();
      TriggerFilter( const std::string & label , const int status = -1 );
      TriggerFilter( const edm::InputTag & tag , const int status = -1 );
      virtual ~TriggerFilter();
      
      /// setters & getters
      void setLabel( std::string & label );
      void setType( std::string & type );
      void addInputCollection( std::string & inputCollection );
      void addObjectId( unsigned int objectId );
      bool setStatus( int status ); // only -1,0,1 accepted; returns 'false' (and does not modify the status) otherwise
      std::string                 label() const;
      std::string                 type() const;
      std::vector< std::string >  inputCollections() const;
      std::string                 inputCollection( const unsigned int i ) const;
      unsigned int                sizeInputCollections() const;
      bool                        hasInputCollection( const std::string & inputCollection ) const;
      std::vector< unsigned int > objectIds() const;                 
      unsigned int                objectId( const unsigned int i ) const;                 
      unsigned int                sizeObjectIds() const;                 
      bool                        hasObjectId( const unsigned int objectId ) const;                 
      int                         status() const;
      
    protected:
    
      /// data members
      std::string                 label_;
      std::string                 type_;
      std::vector< std::string >  inputCollections_; // how to extract this?
      std::vector< unsigned int > objectIds_;        // special filter related object ID as defined in enum 'TriggerObjectType' in DataFormats/HLTReco/interface/TriggerTypeDefs.h
      int                         status_;           // -1: not run, 0: failed, 1: succeeded
        
  };
  

  /// collection of TriggerFilter
  typedef std::vector< TriggerFilter >              TriggerFilterCollection;
  /// persistent reference to an item in a TriggerFilterCollection
  typedef edm::Ref< TriggerFilterCollection >       TriggerFilterRef;
  /// persistent reference to a TriggerFilterCollection product
  typedef edm::RefProd< TriggerFilterCollection >   TriggerFilterRefProd;
  /// vector of persistent references to items in the same TriggerFilterCollection
  typedef edm::RefVector< TriggerFilterCollection > TriggerFilterRefVector;
//   /// persistent reference to an item in a TriggerFilterCollection in the edm::Event
//   typedef edm::Ptr< TriggerFilterCollection >       TriggerFilterPtr;
//   /// vector of persistent references to items in the same TriggerFilterCollection in the edm::Event
//   typedef edm::PtrVector< TriggerFilterCollection > TriggerFilterPtrVector;

}


#endif
