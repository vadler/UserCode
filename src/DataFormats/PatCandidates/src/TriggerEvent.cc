//
// $Id: TriggerEvent.cc,v 1.4 2009/04/28 19:34:19 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"


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

bool TriggerEvent::addObjectMatchResult( const TriggerObjectMatchRefProd & trigMatches, const std::string & labelMatcher )
{
  if ( triggerObjectMatchResults()->find( labelMatcher ) == triggerObjectMatchResults()->end() ) {
    objectMatchResults_[ labelMatcher ] = trigMatches;
    return true;
  }
  edm::LogWarning( "existingObjectMatchResult" ) << "Tried adding trigger object match result from " << labelMatcher << ", although existing.\n"
                                                 << "Skipped.";
  return false;
}
bool TriggerEvent::addObjectMatchResult( const edm::Handle< TriggerObjectMatch > & trigMatches, const std::string & labelMatcher )
{
  return addObjectMatchResult( TriggerObjectMatchRefProd( trigMatches ), labelMatcher );
}
bool TriggerEvent::addObjectMatchResult( const edm::OrphanHandle< TriggerObjectMatch > & trigMatches, const std::string & labelMatcher )
{
  return addObjectMatchResult( TriggerObjectMatchRefProd( trigMatches ), labelMatcher );
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
 
/// x-collection related

TriggerFilterRefVector TriggerEvent::pathModules( const std::string & namePath, bool all ) const
{
  TriggerFilterRefVector thePathFilters;
  if ( path( namePath )->modules().size() == 0 ) {
    return thePathFilters;
  }
  const unsigned onePastLastFilter = all ? path( namePath )->modules().size() : path( namePath )->lastActiveFilterSlot() + 1;
  for ( unsigned iM = 0; iM < onePastLastFilter; ++iM ) {
    const std::string labelFilter( path( namePath )->modules().at( iM ) );
    const TriggerFilterRef filterRef( filters(), indexFilter( labelFilter ) ); // NULL, if filter was not in trigger::TriggerEvent
    thePathFilters.push_back( filterRef );
  }
  return thePathFilters;
}

TriggerFilterRefVector TriggerEvent::pathFilters( const std::string & namePath ) const
{
  TriggerFilterRefVector thePathFilters;
  for ( unsigned iF = 0; iF < path( namePath )->filterIndices().size(); ++iF ) {
    const TriggerFilterRef filterRef( filters(), path( namePath )->filterIndices().at( iF ) );
    thePathFilters.push_back( filterRef );
  }
  return thePathFilters;
}

bool TriggerEvent::filterInPath( const TriggerFilterRef & filterRef, const std::string & namePath ) const
{
  TriggerFilterRefVector theFilters = pathFilters( namePath );
  for ( TriggerFilterRefVector::const_iterator iFilter = theFilters.begin(); iFilter != theFilters.end(); ++iFilter ) {
    if ( filterRef == *iFilter ) {
      return true;
    }
  }
  return false;
}

TriggerPathRefVector TriggerEvent::filterPaths( const TriggerFilterRef & filterRef ) const
{
  TriggerPathRefVector theFilterPaths;
  size_t cPaths( 0 );
  for ( TriggerPathCollection::const_iterator iPath = paths()->begin(); iPath != paths()->end(); ++iPath ) {
    const std::string namePath( iPath->name() );
    if ( filterInPath( filterRef, namePath ) ) {
      const TriggerPathRef pathRef( paths(), cPaths );
      theFilterPaths.push_back( pathRef );
    }
    ++cPaths;
  }
  return theFilterPaths;
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
  TriggerFilterRefVector theFilters = pathFilters( namePath );
  for ( TriggerFilterRefVector::const_iterator iFilter = theFilters.begin(); iFilter != theFilters.end(); ++iFilter ) {
    const std::string labelFilter( ( *iFilter )->label() );
    TriggerObjectRefVector theObjects = filterObjects( labelFilter );
    for ( TriggerObjectRefVector::const_iterator iObject = theObjects.begin(); iObject != theObjects.end(); ++iObject ) {
      thePathObjects.push_back( *iObject );
    }
  }
  return thePathObjects;
}

bool TriggerEvent::objectInPath( const TriggerObjectRef & objectRef, const std::string & namePath ) const
{
  TriggerFilterRefVector theFilters = pathFilters( namePath );
  for ( TriggerFilterRefVector::const_iterator iFilter = theFilters.begin(); iFilter != theFilters.end(); ++iFilter ) {
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

std::vector< std::string > TriggerEvent::triggerMatchers() const
{
  std::vector< std::string > theMatchers;
  for ( TriggerObjectMatchContainer::const_iterator iMatch = triggerObjectMatchResults()->begin(); iMatch != triggerObjectMatchResults()->end(); ++iMatch ) {
    theMatchers.push_back( iMatch->first );
  }
  return theMatchers;
}

const TriggerObjectMatch * TriggerEvent::triggerObjectMatchResult( const std::string & labelMatcher ) const
{
  const TriggerObjectMatchContainer::const_iterator iMatch( triggerObjectMatchResults()->find( labelMatcher ) );
  if ( iMatch != triggerObjectMatchResults()->end() ) {
    return iMatch->second.get();
  }
  return 0;
}
