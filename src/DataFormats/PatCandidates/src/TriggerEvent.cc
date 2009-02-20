//
// $Id: TriggerEvent.cc,v 1.1.2.1 2008/12/18 11:26:16 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "DataFormats/Common/interface/AssociativeIterator.h"


using namespace pat;


/// constructor from values
TriggerEvent::TriggerEvent( const std::string & nameHltTable, bool run, bool accept, bool error ) :
  nameHltTable_( nameHltTable ),
  run_( run ),
  accept_( accept ),
  error_( error )
{
  objectMatchResults_.clear();
}

/// paths related

void TriggerEvent::setPaths( const edm::Handle< TriggerPathCollection > & handleTriggerPaths )
{
  paths_ = TriggerPathRefProd( handleTriggerPaths );
}

const TriggerPathCollection * TriggerEvent::paths() const
{
  return paths_.get();
}

const TriggerPath * TriggerEvent::path( const std::string & namePath ) const
{
  for ( TriggerPathCollection::const_iterator iPath = paths()->begin(); iPath != paths()->end(); ++iPath ) {
    if ( namePath == iPath->name() ) {
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
      const TriggerPathRef pathRef( paths(), indexPath( namePath ) );
      theAcceptedPaths.push_back( pathRef );
    }
  }
  return theAcceptedPaths;
}
      
/// filters related

void TriggerEvent::setFilters( const edm::Handle< TriggerFilterCollection > & handleTriggerFilters )
{
  filters_ = TriggerFilterRefProd( handleTriggerFilters );
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
      const TriggerFilterRef filterRef( filters(), indexFilter( labelFilter ) );
      theAcceptedFilters.push_back( filterRef );
    }
  }
  return theAcceptedFilters;
}

/// objects related

void TriggerEvent::setObjects( const edm::Handle< TriggerObjectCollection > & handleTriggerObjects )
{
  objects_ = TriggerObjectRefProd( handleTriggerObjects );
}

bool TriggerEvent::addObjectMatchResult( const TriggerObjectMatch & trigMatches, const std::string & labelMatcher )
{
  if ( objectMatchResults()->find( labelMatcher ) == objectMatchResults()->end() ) {
    objectMatchResults_[ labelMatcher ] = trigMatches;
    return true;
  }
  return false;
}

const TriggerObjectCollection * TriggerEvent::objects() const
{
  return objects_.get();
}

TriggerObjectRefVector TriggerEvent::objects( unsigned filterId ) const
{
  TriggerObjectRefVector theObjects;
  for ( unsigned iObject = 0; iObject < objects()->size(); ++iObject ) {
    if ( objects()->at( iObject ).hasFilterId( filterId ) ) {
      const TriggerObjectRef objectRef( objects(), iObject );
      theObjects.push_back( objectRef );
    }
  }
  return theObjects;
}

const TriggerObjectMatchContainer * TriggerEvent::objectMatchResults() const
{
  return &objectMatchResults_;
}

// checks existance of key with std::map::find() and returns an unvalid pointer 
// instead of adding a new element to the map and returning a valid but empty pointer by using operator[]
const TriggerObjectMatch * TriggerEvent::objectMatchResult( const std::string & labelMatcher ) const
{
  const TriggerObjectMatchContainer::const_iterator iMatch( objectMatchResults()->find( labelMatcher ) );
  if ( iMatch != objectMatchResults()->end() ) {
    return &( iMatch->second );
  }
  return 0;
}
 
/// x-collection related

TriggerFilterRefVector TriggerEvent::pathFilters( const std::string & namePath, bool all ) const
{
  TriggerFilterRefVector thePathFilters;
  const unsigned lastModule = all ? path( namePath )->modules().size() : path( namePath )->lastActiveModule();
  for ( unsigned iM = 0; iM < lastModule; ++iM ) {
    const std::string labelFilter( path( namePath )->modules().at( iM ) );
    const TriggerFilterRef filterRef( filters(), indexFilter( labelFilter ) );
    thePathFilters.push_back( filterRef );
  }
  return thePathFilters;
}

bool TriggerEvent::filterInPath( const TriggerFilterRef & filterRef, const std::string & namePath, bool all ) const
{
  TriggerFilterRefVector filterVector( pathFilters( namePath, all ) );
  for ( TriggerFilterRefVector::const_iterator iFilter = filterVector.begin(); iFilter != filterVector.end(); ++iFilter ) {
    if ( filterRef == *iFilter ) {
      return true;
    }
  }
  return false;
}

TriggerPathRefVector TriggerEvent::filterPaths( const TriggerFilterRef & filterRef ) const
{
  return TriggerPathRefVector();
}

std::vector< std::string > TriggerEvent::filterCollections( const std::string & labelFilter ) const
{
  std::vector< std::string > theFilterCollections;
  for ( unsigned iObject = 0; iObject < objects()->size(); ++iObject ) {
    if ( filter( labelFilter )->hasObjectKey( iObject ) ) {
      bool found( false );
      std::string objectCollection( objects()->at( iObject ).collection() );
      for ( std::vector< std::string >::const_iterator iC = theFilterCollections.begin(); iC != theFilterCollections.end(); ++iC ) {
        if ( *iC == objectCollection ) {
          found = true;
          break;
        }
      }
      if ( ! found ) {
        theFilterCollections.push_back( objectCollection );
      }
    }
  }
  return theFilterCollections;
}

TriggerObjectRefVector TriggerEvent::filterObjects( const std::string & labelFilter ) const
{
  TriggerObjectRefVector theFilterObjects;
  for ( unsigned iObject = 0; iObject < objects()->size(); ++iObject ) {
    if ( filter( labelFilter )->hasObjectKey( iObject ) ) {
      const TriggerObjectRef objectRef( objects(), iObject );
      theFilterObjects.push_back( objectRef );
    }
  }
  return theFilterObjects;
}

bool TriggerEvent::objectInFilter( const TriggerObjectRef & objectRef, const std::string & labelFilter ) const {
  return filter( labelFilter )->hasObjectKey( objectRef.key() );
}
                                                 
TriggerFilterRefVector TriggerEvent::objectFilters( const TriggerObjectRef & objectRef ) const
{
  TriggerFilterRefVector theObjectFilters;
  for ( TriggerFilterCollection::const_iterator iFilter = filters()->begin(); iFilter != filters()->end(); ++iFilter ) {
    const std::string labelFilter( iFilter->label() );
    if ( objectInFilter( objectRef, labelFilter ) ) {
      const TriggerFilterRef filterRef( filters(), indexFilter( labelFilter ) );
      theObjectFilters.push_back( filterRef );
    }
  }
  return theObjectFilters;
}

TriggerObjectRefVector TriggerEvent::pathObjects( const std::string & namePath ) const
{
  TriggerObjectRefVector thePathObjects;
  for ( TriggerFilterRefVector::const_iterator iFilter = pathFilters( namePath ).begin(); iFilter != pathFilters( namePath ).end(); ++iFilter ) {
    const std::string labelFilter( ( *iFilter )->label() );
    for ( TriggerObjectRefVector::const_iterator iObject = filterObjects( labelFilter ).begin(); iObject != filterObjects( labelFilter ).end(); ++iObject ) {
      thePathObjects.push_back( *iObject );
    }
  }
  return thePathObjects;
}

bool TriggerEvent::objectInPath( const TriggerObjectRef & objectRef, const std::string & namePath ) const
{
  for ( TriggerFilterRefVector::const_iterator iFilter = pathFilters( namePath ).begin(); iFilter != pathFilters( namePath ).end(); ++iFilter ) {
    if ( objectInFilter( objectRef, ( *iFilter )->label() ) ) {
      return true;
    }
  }
  return false;
}

TriggerPathRefVector TriggerEvent::objectPaths( const TriggerObjectRef & objectRef ) const
{
  TriggerPathRefVector theObjectPaths;
  for ( TriggerPathCollection::const_iterator iPath = paths()->begin(); iPath != paths()->end(); ++iPath ) {
    const std::string namePath( iPath->name() );
    if ( objectInPath( objectRef, namePath ) ) {
      const TriggerPathRef pathRef( paths(), indexPath( namePath ) );
      theObjectPaths.push_back( pathRef );
    }
  }
  return theObjectPaths;
}

/// trigger matches

TriggerObjectMatchMap TriggerEvent::triggerMatchObjects( const reco::CandidateBaseRef & candRef ) const
{
  TriggerObjectMatchMap theContainer;
  for ( TriggerObjectMatchContainer::const_iterator iMatch = objectMatchResults()->begin(); iMatch != objectMatchResults()->end(); ++iMatch ) {
    theContainer[ iMatch->first ] = triggerMatchObject( candRef, iMatch->first );
  }
  return theContainer;
}

TriggerObjectRef TriggerEvent::triggerMatchObject( const reco::CandidateBaseRef & candRef, const std::string & labelMatcher ) const
{
  const TriggerObjectMatch * matchResult( objectMatchResult( labelMatcher ) );
  if ( matchResult ) {
    TriggerObjectRef objectRef( ( *matchResult )[ candRef ] );
    if ( objectRef.isNonnull() && objectRef.isAvailable() ) {
      return objectRef;
    }
  }
  return TriggerObjectRef();
}

// Finds matching candidates by iterating over edm::Association, since this in on-directional;
// The used edm::AssiciativIterator needs the event!
reco::CandidateBaseRefVector TriggerEvent::triggerMatchCandidates( const TriggerObjectRef & objectRef, const std::string & labelMatcher, const edm::Event & iEvent ) const
{
  reco::CandidateBaseRefVector theCands;
  edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectMatch > it( *( objectMatchResult( labelMatcher ) ), edm::EdmEventItemGetter< reco::CandidateBaseRef >( iEvent ) );
  edm::AssociativeIterator< reco::CandidateBaseRef, TriggerObjectMatch > itEnd( it.end() );
  for ( ; it != itEnd; ++it ) {
    if ( it->first.isNonnull() && it->second.isNonnull() && it->second.isAvailable() ) {
      if ( it->second == objectRef ) {
        theCands.push_back( it->first );
      }
    }
  }
  return theCands;
}
