//
// $Id$
//


#include "DataFormats/PatCandidates/interface/TriggerFilter.h"


using namespace pat;

/// default constructor

TriggerFilter::TriggerFilter() :
  label_(),
  type_(),
  status_()
{
  inputCollections_.clear();
  objectIds_.clear();
}

/// constructor from values

TriggerFilter::TriggerFilter( const std::string & label , const int status ) :
  label_( label ),
  type_(),
  status_( status )
{
  inputCollections_.clear();
  objectIds_.clear();
}

TriggerFilter::TriggerFilter( const edm::InputTag & tag , const int status ) :
  label_( tag.label() ),
  type_(),
  status_( status )
{
  inputCollections_.clear();
  objectIds_.clear();
}

/// destructor

TriggerFilter::~TriggerFilter()
{
}

/// setters

void TriggerFilter::setLabel( std::string & label )
{
  label_ = label;
}

void TriggerFilter::setType( std::string & type )
{
  type_ = type;
}

void TriggerFilter::addInputCollection( std::string & inputCollection )
{
  inputCollections_.push_back( inputCollection );
}

void TriggerFilter::addObjectId( unsigned int objectId )
{
  objectIds_.push_back( objectId );
}

bool TriggerFilter::setStatus( int status )
{
  if ( status < -1 || 1 < status ) {
    return false;
  }
  status_ = status;
  return true;
}

/// getters

std::string TriggerFilter::label() const
{
  return label_;
}

std::string TriggerFilter::type() const
{
  return type_;
}

std::vector< std::string > TriggerFilter::inputCollections() const
{
  return inputCollections_;
}

std::string TriggerFilter::inputCollection( const unsigned int i ) const
{
  return inputCollections_[ i ];
}

unsigned int TriggerFilter::sizeInputCollections() const
{
  return inputCollections_.size();
}

bool TriggerFilter::hasInputCollection( const std::string & inputCollection ) const
{
  bool found = false;
  for ( std::vector< std::string >::const_iterator iI = inputCollections_.begin(); iI != inputCollections_.end(); ++iI ) {
    if ( *iI == inputCollection ) {
      found = true;
      break;
    }
  }
  return found;
}

std::vector< unsigned int > TriggerFilter::objectIds() const
{
  return objectIds_;
}

unsigned int TriggerFilter::objectId( const unsigned int i ) const
{
  return objectIds_[ i ];
}

unsigned int TriggerFilter::sizeObjectIds() const
{
  return objectIds_.size();
}

bool TriggerFilter::hasObjectId( const unsigned int objectId ) const
{
  bool found = false;
  for ( std::vector< unsigned int >::const_iterator iO = objectIds_.begin(); iO != objectIds_.end(); ++iO ) {
    if ( *iO == objectId ) {
      found = true;
      break;
    }
  }
  return found;
}

int TriggerFilter::status() const
{
  return status_;
}
