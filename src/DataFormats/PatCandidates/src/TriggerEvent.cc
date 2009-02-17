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
  const TriggerPathCollection * allPaths( paths() );
  for ( TriggerPathCollection::const_iterator iP = allPaths->begin(); iP != allPaths->end(); ++iP ) {
    if ( iP->name() == namePath ) {
      return &*iP;
    }
  }
  return 0;
}

unsigned TriggerEvent::indexPath( const std::string & namePath ) const
{
  const TriggerPathCollection * allPaths( paths() );
  unsigned iP = 0;
  while ( iP < allPaths->size() && allPaths->at( iP ).name() != namePath ) {
    ++iP;
  }
  return iP;
}

TriggerPathRefVector TriggerEvent::acceptedPaths() const
{
  const TriggerPathCollection   * allPaths( paths() );
  TriggerPathRefVector            goodPaths;
  for ( unsigned iP = 0; iP < allPaths->size(); ++iP ) {
    const TriggerPath thisPath( allPaths->at( iP ) );
    if ( thisPath.wasAccept() ) {
      const std::string namePath( thisPath.name() );
      const unsigned keyPath( indexPath( namePath ) );
      const TriggerPathRef thisPathRef( allPaths, keyPath );
      goodPaths.push_back( thisPathRef );
    }
  }
  return goodPaths;
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
  const TriggerFilterCollection * allFilters( filters() );
  for ( TriggerFilterCollection::const_iterator iF = allFilters->begin(); iF != allFilters->end(); ++iF ) {
    if ( iF->label() == labelFilter ) {
      return &*iF;
    }
  }
  return 0;
}

unsigned TriggerEvent::indexFilter( const std::string & labelFilter ) const
{
  const TriggerFilterCollection * allFilters( filters() );
  unsigned iF = 0;
  while ( iF < allFilters->size() && allFilters->at( iF ).label() != labelFilter ) {
    ++iF;
  }
  return iF;
}

TriggerFilterRefVector TriggerEvent::acceptedFilters() const
{
  const TriggerFilterCollection   * allFilters( filters() );
  TriggerFilterRefVector            goodFilters;
  for ( unsigned iF = 0; iF < allFilters->size(); ++iF ) {
    const TriggerFilter thisFilter( allFilters->at( iF ) );
    if ( thisFilter.status() == 1 ) {
      const std::string labelFilter( thisFilter.label() );
      const unsigned keyFilter( indexFilter( labelFilter ) );
      const TriggerFilterRef thisFilterRef( allFilters, keyFilter );
      goodFilters.push_back( thisFilterRef );
    }
  }
  return goodFilters;
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
  const TriggerPath             * thisPath( path( namePath ) );
  const TriggerFilterCollection * allFilters( filters() );
  TriggerFilterRefVector          pathFilters;
  const unsigned lastModule = all ? thisPath->modules().size() : thisPath->lastActiveModule();
  for ( unsigned iM = 0; iM < lastModule; ++iM ) {
    const std::string labelFilter( thisPath->modules().at( iM ) );
    const unsigned keyFilter( indexFilter( labelFilter ) );
    const TriggerFilterRef thisFilterRef( allFilters, keyFilter );
    pathFilters.push_back( thisFilterRef );
  }
  return pathFilters;
}

std::vector< std::string > TriggerEvent::filterCollections( const std::string & labelFilter ) const
{
  const TriggerObjectCollection * allObjects( objects() );
  std::vector< std::string >      filterCollections;
  for ( unsigned iO = 0; iO < allObjects->size(); ++iO ) {
    if ( filter( labelFilter )->hasObjectKey( iO ) ) {
      std::string thisCollection( allObjects->at( iO ).collection() );
      bool found = false;
      for ( std::vector< std::string >::const_iterator iC = filterCollections.begin(); iC != filterCollections.end(); ++iC ) {
        if ( *iC == thisCollection ) {
          found = true;
          break;
        }
      }
      if ( ! found ) {
        filterCollections.push_back( thisCollection );
      }
    }
  }
  return filterCollections;
}

TriggerObjectRefVector TriggerEvent::filterObjects( const std::string & labelFilter ) const
{
  const TriggerFilter           * thisFilter( filter( labelFilter ) );
  const TriggerObjectCollection * allObjects( objects() );
  TriggerObjectRefVector          filterObjects;
  for ( unsigned iO = 0; iO < allObjects->size(); ++iO ) {
    if ( thisFilter->hasObjectKey( iO ) ) {
      const TriggerObjectRef thisObjectRef( allObjects, iO );
      filterObjects.push_back( thisObjectRef );
    }
  }
  return filterObjects;
}

TriggerObjectRefVector TriggerEvent::pathObjects( const std::string & namePath ) const
{
  const TriggerFilterRefVector theseFilters( pathFilters( namePath ) );
  TriggerObjectRefVector       pathObjects;
  for ( TriggerFilterRefVector::const_iterator iF = theseFilters.begin(); iF != theseFilters.end(); ++iF ) {
    const std::string labelFilter( ( *iF )->label() );
    TriggerObjectRefVector theseObjects( filterObjects( labelFilter ) );
    for ( TriggerObjectRefVector::const_iterator iO = theseObjects.begin(); iO != theseObjects.end(); ++iO ) {
      pathObjects.push_back( *iO );
    }
  }
  return pathObjects;
}
