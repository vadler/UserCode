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
  objectIds_.clear();
}

/// constructor from values

TriggerFilter::TriggerFilter( const std::string & label , const int status ) :
  label_( label ),
  type_(),
  status_( status )
{
  objectIds_.clear();
}

TriggerFilter::TriggerFilter( const edm::InputTag & tag , const int status ) :
  label_( tag.label() ),
  type_(),
  status_( status )
{
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
