//
// $Id: TriggerObjectStandAlone.cc,v 1.6 2010/12/14 19:31:36 vadler Exp $
//

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include <iostream> // DEBUG

#include <boost/algorithm/string.hpp>


using namespace pat;


// Const data members' definitions


const char TriggerObjectStandAlone::wildcard_;


// Private methods


// Checks a string vector for occurence of a certain string, incl. wild-card mechanism
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


// Methods


// Adds a new path name
void TriggerObjectStandAlone::addPathName( const std::string & pathName, bool pathLastFilterAccepted )
{
  // Check, if path is already assigned
  if ( ! hasPathName( pathName, false ) ) {
    // The path itself
    pathNames_.push_back( pathName );
    // The corresponding usage of the trigger objects
    pathLastFilterAccepted_.push_back( pathLastFilterAccepted );
  }
}


// Gets all path names
std::vector< std::string > TriggerObjectStandAlone::pathNames( bool pathLastFilterAccepted ) const
{
  // All path names, if usage not restricted (not required or not available)
  if ( ! pathLastFilterAccepted || ! hasPathLastFilterAccepted() ) return pathNames_;
  // Temp vector of path names
  std::vector< std::string > paths;
  // Loop over usage vector and fill corresponding paths into temp vector
  for ( unsigned iPath = 0; iPath < pathNames_.size(); ++iPath ) {
    if ( pathLastFilterAccepted_.at( iPath ) ) paths.push_back( pathNames_.at( iPath ) );
  }
  // Return temp vector
  return paths;
}


// Gets the pat::TriggerObject (parent class)
TriggerObject TriggerObjectStandAlone::triggerObject()
{
  // Create a TriggerObjects
  TriggerObject theObj( p4(), pdgId() );
  // Set its collection and trigger objects types (no c'tor for that)
  theObj.setCollection( collection() );
  for ( size_t i = 0; i < filterIds().size(); ++i ) theObj.addFilterId( filterIds().at( i ) );
  // Return TriggerObject
  return theObj;
}


// Checks, if a certain filter label is assigned
bool TriggerObjectStandAlone::hasFilterLabel( const std::string & filterLabel ) const
{
  // Move to wild-card parser, if needed
  if ( filterLabel.find( wildcard_ ) != std::string::npos ) return hasAnyName( filterLabel, filterLabels_ );
  // Return, if filter label is assigned
  return ( std::find( filterLabels_.begin(), filterLabels_.end(), filterLabel ) != filterLabels_.end() );
}


// Checks, if a certain path name is assigned
bool TriggerObjectStandAlone::hasPathName( const std::string & pathName, bool pathLastFilterAccepted ) const
{
  // Move to wild-card parser, if needed
  if ( pathName.find( wildcard_ ) != std::string::npos ) return hasAnyName( pathName, pathNames( pathLastFilterAccepted ) );
  // Deal with older PAT-tuples, where trigger object usage is not available
  if ( ! hasPathLastFilterAccepted() ) pathLastFilterAccepted = false;
  // Check, if path name is assigned at all
  std::vector< std::string >::const_iterator match( std::find( pathNames_.begin(), pathNames_.end(), pathName ) );
  // False, if path name not assigned
  if ( match == pathNames_.end() ) return false;
  // Return for assigned path name, if trigger object usage meets requirement
  return ( pathLastFilterAccepted ? pathLastFilterAccepted_.at( match - pathNames_.begin() ) : true );
}


// Checks, if a certain label of original collection is assigned (method overrides)
bool TriggerObjectStandAlone::hasCollection( const std::string & coll ) const
{
  // Move to wild-card parser, if needed
  if ( coll.find( wildcard_ ) != std::string::npos ) return false; // FIXME: dummy
  // Use parent class's method other wise
  return TriggerObject::hasCollection( coll );
}
