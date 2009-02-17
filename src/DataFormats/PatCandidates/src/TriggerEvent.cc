//
// $Id: TriggerEvent.cc,v 1.1.2.1 2008/12/18 11:26:16 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerEvent.h"


using namespace pat;

/// default constructor
TriggerEvent::TriggerEvent() {}

/// constructor from values
TriggerEvent::TriggerEvent( const std::string & nameHltTable, bool run, bool accept, bool error ) :
  nameHltTable_( nameHltTable ),
  run_( run ),
  accept_( accept ),
  error_( error )
{
}

/// destructor
TriggerEvent::~TriggerEvent()
{
}

/// event related

void TriggerEvent::setNameHltTable( const std::string & nameHltTable )
{
  nameHltTable_ = nameHltTable;
}

void TriggerEvent::setRun( bool run )
{
  run_ = run;
}

void TriggerEvent::setAccept( bool accept )
{
  accept_ = accept;
}

void TriggerEvent::setError( bool error )
{
  error_ = error;
}

std::string TriggerEvent::nameHltTable() const
{
  return nameHltTable_;
}

bool TriggerEvent::wasRun() const
{
  return run_;
}

bool TriggerEvent::wasAccept() const
{
  return accept_;
}

bool TriggerEvent::wasError() const
{
  return error_;
}

/// paths related

void TriggerEvent::setPathCollection( const edm::Handle< TriggerPathCollection > & handleTriggerPaths )
{
  setPaths( handleTriggerPaths );
}
void TriggerEvent::setPaths( const edm::Handle< TriggerPathCollection > & handleTriggerPaths )
{
  paths_ = TriggerPathRefProd( handleTriggerPaths );
}

const TriggerPathCollection * TriggerEvent::pathCollection() const
{
  return paths();
}
const TriggerPathCollection * TriggerEvent::paths() const
{
  return paths_.get();
}

const TriggerPath * TriggerEvent::path( const std::string & namePath ) const
{
  for ( TriggerPathCollection::const_iterator iPath = paths()->begin(); iPath != paths()->end(); ++iPath ) {
    if ( iPath->name() == namePath ) {
      return &*iPath;
    }
  }
  return 0;
}

unsigned TriggerEvent::indexPath( const std::string & namePath ) const
{
  unsigned iPath = 0;
  while ( iPath < paths()->size() && paths()->at( iPath ).name() != namePath ) {
    ++iPath;
  }
  return iPath;
}

TriggerPathRefVector TriggerEvent::acceptedPaths() const
{
  TriggerPathRefVector theAcceptedPaths;
  for ( TriggerPathCollection::const_iterator iPath = paths()->begin(); iPath != paths()->end(); ++iPath ) {
    if ( iPath->wasAccept() ) {
      const std::string namePath( iPath->name() );
      const unsigned keyPath( indexPath( namePath ) );
      const TriggerPathRef aPathRef( paths(), keyPath );
      theAcceptedPaths.push_back( aPathRef );
    }
  }
  return theAcceptedPaths;
}
      
/// filters related

void TriggerEvent::setFilterCollection( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters )
{
  setFilters( handleTriggerFilters );
}
void TriggerEvent::setFilters( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters )
{
  filters_ = TriggerFilterRefProd( handleTriggerFilters );
}

const TriggerFilterCollection * TriggerEvent::filterCollection() const
{
  return filters();
}
const TriggerFilterCollection * TriggerEvent::filters() const
{
  return filters_.get();
}

const TriggerFilter * TriggerEvent::filter( const std::string & labelFilter ) const
{
  for ( TriggerFilterCollection::const_iterator iFilter = filters()->begin(); iFilter != filters()->end(); ++iFilter ) {
    if ( iFilter->label() == labelFilter ) {
      return &*iFilter;
    }
  }
  return 0;
}

unsigned TriggerEvent::indexFilter( const std::string & labelFilter ) const
{
  unsigned iFilter = 0;
  while ( iFilter < filters()->size() && filters()->at( iFilter ).label() != labelFilter ) {
    ++iFilter;
  }
  return iFilter;
}

TriggerFilterRefVector TriggerEvent::acceptedFilters() const
{
  TriggerFilterRefVector theAcceptedFilters;
  for ( TriggerFilterCollection::const_iterator iFilter = filters()->begin(); iFilter != filters()->end(); ++iFilter ) {
    if ( iFilter->status() == 1 ) {
      const std::string labelFilter( iFilter->label() );
      const unsigned keyFilter( indexFilter( labelFilter ) );
      const TriggerFilterRef filterRef( filters(), keyFilter );
      theAcceptedFilters.push_back( filterRef );
    }
  }
  return theAcceptedFilters;
}

/// objects related

void TriggerEvent::setObjectCollection( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects )
{
  setObjects( handleTriggerObjects );
}
void TriggerEvent::setObjects( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects )
{
  objects_ = TriggerObjectRefProd( handleTriggerObjects );
}
void TriggerEvent::addObjectMatchResult( const TriggerObjectMatches & trigMatches, const std::string & matcher ) {
  TriggerEvent::TriggerObjectMatchResult objectMatchResult( trigMatches, matcher );
  objectMatchResults_.push_back( objectMatchResult );
}

const TriggerObjectCollection * TriggerEvent::objectCollection() const
{
  return objects();
}
const TriggerObjectCollection * TriggerEvent::objects() const
{
  return objects_.get();
}
 
/// x-collection related

TriggerFilterRefVector TriggerEvent::pathFilters( const std::string & namePath, bool all ) const
{
  TriggerFilterRefVector thePathFilters;
  const unsigned lastModule = all ? path( namePath )->modules().size() : path( namePath )->lastActiveModule();
  for ( unsigned iM = 0; iM < lastModule; ++iM ) {
    const std::string labelFilter( path( namePath )->modules().at( iM ) );
    const unsigned keyFilter( indexFilter( labelFilter ) );
    const TriggerFilterRef filterRef( filters(), keyFilter );
    thePathFilters.push_back( filterRef );
  }
  return thePathFilters;
}

std::vector< std::string > TriggerEvent::filterCollections( const std::string & labelFilter ) const
{
  std::vector< std::string > theFilterCollections;
  for ( unsigned iO = 0; iO < objects()->size(); ++iO ) {
    if ( filter( labelFilter )->hasObjectKey( iO ) ) {
      std::string collection( objects()->at( iO ).collection() );
      bool found = false;
      for ( std::vector< std::string >::const_iterator iC = theFilterCollections.begin(); iC != theFilterCollections.end(); ++iC ) {
        if ( *iC == collection ) {
          found = true;
          break;
        }
      }
      if ( ! found ) {
        theFilterCollections.push_back( collection );
      }
    }
  }
  return theFilterCollections;
}

TriggerObjectRefVector TriggerEvent::filterObjects( const std::string & labelFilter ) const
{
  TriggerObjectRefVector theFilterObjects;
  for ( unsigned iO = 0; iO < objects()->size(); ++iO ) {
    if ( filter( labelFilter )->hasObjectKey( iO ) ) {
      const TriggerObjectRef aObjectRef( objects(), iO );
      theFilterObjects.push_back( aObjectRef );
    }
  }
  return theFilterObjects;
}

bool TriggerEvent::objectInFilter( const TriggerObjectRef objectRef, const std::string & labelFilter ) const {
  return filter( labelFilter )->hasObjectKey( objectRef.key() );
}

TriggerObjectRefVector TriggerEvent::pathObjects( const std::string & namePath ) const
{
  TriggerObjectRefVector thePathObjects;
  for ( TriggerFilterRefVector::const_iterator iFilter = pathFilters( namePath ).begin(); iFilter != pathFilters( namePath ).end(); ++iFilter ) {
    const std::string labelFilter( ( *iFilter )->label() );
    for ( TriggerObjectRefVector::const_iterator iO = filterObjects( labelFilter ).begin(); iO != filterObjects( labelFilter ).end(); ++iO ) {
      thePathObjects.push_back( *iO );
    }
  }
  return thePathObjects;
}

bool TriggerEvent::objectInPath( const TriggerObjectRef objectRef, const std::string & namePath ) const {
  for ( TriggerFilterRefVector::const_iterator iFilter = pathFilters( namePath ).begin(); iFilter != pathFilters( namePath ).end(); ++iFilter ) {
    if ( ( *iFilter )->hasObjectKey( objectRef.key() ) ) {
      return true;
    }
  }
  return false;
}
                                                 
TriggerFilterRefVector TriggerEvent::objectFilters( const TriggerObjectRef objectRef ) const {
  TriggerFilterRefVector theObjectFilters;
  for ( TriggerFilterCollection::const_iterator iFilter = filters()->begin(); iFilter != filters()->end(); ++iFilter ) {
    const std::string labelFilter( iFilter->label() );
    if ( objectInFilter( objectRef, labelFilter ) ) {
      const unsigned keyFilter( indexFilter( labelFilter ) );
      const TriggerFilterRef filterRef( filters(), keyFilter );
      theObjectFilters.push_back( filterRef );
    }
  }
  return theObjectFilters;
}

TriggerPathRefVector TriggerEvent::objectPaths( const TriggerObjectRef objectRef ) const {
  return TriggerPathRefVector();
}
