//
// $Id: TriggerObjectStandAlone.cc,v 1.6 2010/12/14 19:31:36 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include <iostream> // DEBUG

#include <boost/algorithm/string.hpp>


using namespace pat;


/// Const data members' definitions


const char TriggerObjectStandAlone::wildcard_;


/// Private methods


bool TriggerObjectStandAlone::hasAnyName( const std::string & name, const std::vector< std::string > & nameVec ) const
{
  // Split name to evaluate in parts, seperated by wild-cards
  std::vector< std::string > namePartsVec;
  boost::split( namePartsVec, name, boost::is_any_of( std::string( 1, wildcard_ ) ), boost::token_compress_on );
  // Iterate over vector of names to search
  for ( std::vector< std::string >::const_iterator iVec = nameVec.begin(); iVec != nameVec.end(); ++iVec ) {
    // Not failed yet
    bool failed( false );
    // Index to start from
    unsigned index( 0 );
    // Iterate over evaluation name parts
    for ( std::vector< std::string >::const_iterator iName = namePartsVec.begin(); iName != namePartsVec.end(); ++iName ) {
      // Empty parts due to
      // - wild-card at beginning/end or
      // - multiple wild-cards (should be supressed by 'boost::token_compress_on')
      if ( iName->length() == 0 ) continue;
      // Search from current index and
      // set index to found occurence
      index = iVec->find( *iName, index );
      // Failed and exit loop, if
      // - part not found
      // - part at beginning not found there
      if ( index == std::string::npos || ( iName == namePartsVec.begin() && index > 0 ) ) {
        failed = true;
        break;
      }
      // Increase index by length of found part
      index += iName->length();
    }
    // Failed, if end of name not reached
    if ( index < iVec->length() && namePartsVec.back().length() != 0 ) failed = true;
    // Match found!
    if ( ! failed ) return true;
  }
  // No match found!
  return false;
}


/// Methods


void TriggerObjectStandAlone::addPathName( const std::string & pathName, bool pathLastFilterAccepted )
{
  if ( ! hasPathName( pathName, false ) ) {
    pathNames_.push_back( pathName );
    pathLastFilterAccepted_.push_back( pathLastFilterAccepted );
  }
}


std::vector< std::string > TriggerObjectStandAlone::pathNames( bool pathLastFilterAccepted ) const
{
  if ( ! pathLastFilterAccepted || ! hasPathLastFilterAccepted() ) return pathNames_;
  std::vector< std::string > paths;
  for ( unsigned iPath = 0; iPath < pathNames_.size(); ++iPath ) {
    if ( pathLastFilterAccepted_.at( iPath ) ) paths.push_back( pathNames_.at( iPath ) );
  }
  return paths;
}


TriggerObject TriggerObjectStandAlone::triggerObject()
{
  TriggerObject theObj( p4(), pdgId() );
  theObj.setCollection( collection() );
  for ( size_t i = 0; i < filterIds().size(); ++i ) theObj.addFilterId( filterIds().at( i ) );
  return theObj;
}


bool TriggerObjectStandAlone::hasFilterLabel( const std::string & filterLabel ) const
{
  if ( filterLabel.find( wildcard_ ) != std::string::npos ) return hasAnyName( filterLabel, filterLabels_ );
  return ( std::find( filterLabels_.begin(), filterLabels_.end(), filterLabel ) != filterLabels_.end());
}


bool TriggerObjectStandAlone::hasPathName( const std::string & pathName, bool pathLastFilterAccepted ) const
{
  if ( pathName.find( wildcard_ ) != std::string::npos ) return hasAnyName( pathName, pathNames( pathLastFilterAccepted ) );
  if ( ! hasPathLastFilterAccepted() ) pathLastFilterAccepted = false;
  std::vector< std::string >::const_iterator match( std::find( pathNames_.begin(), pathNames_.end(), pathName ) );
  if ( match == pathNames_.end() ) return false;
  return ( pathLastFilterAccepted ? pathLastFilterAccepted_.at( match - pathNames_.begin() ) : true );
}


bool TriggerObjectStandAlone::hasCollection( const std::string & coll ) const
{
  if ( coll.find( wildcard_ ) != std::string::npos ) return false; // FIXME: dummy
  if ( collection() == coll ) return true;
  const edm::InputTag collectionTag( collection() );
  const edm::InputTag collTag( coll );
  if ( collTag.process().empty() ) {
    if ( ( collTag.instance().empty() && collectionTag.instance().empty() ) || collTag.instance() == collectionTag.instance() ) {
      if ( collTag.label() == collectionTag.label() ) return true;
    }
  }
  return false;
}
