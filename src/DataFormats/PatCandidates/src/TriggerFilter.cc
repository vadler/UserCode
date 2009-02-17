//
// $Id: TriggerFilter.cc,v 1.1.2.1 2008/12/18 11:26:16 vadler Exp $
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

TriggerFilter::TriggerFilter( const std::string & label, int status ) :
  label_( label ),
  type_(),
  status_( status )
{
  objectIds_.clear();
}

TriggerFilter::TriggerFilter( const edm::InputTag & tag, int status ) :
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

void TriggerFilter::setLabel( const std::string & label )
{
  label_ = label;
}

void TriggerFilter::setType( const std::string & type )
{
  type_ = type;
}

void TriggerFilter::addObjectKey( unsigned objectKey )
{
  objectKeys_.push_back( objectKey );
}

void TriggerFilter::addObjectId( unsigned objectId )
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

std::vector< unsigned > TriggerFilter::objectKeys() const
{
  return objectKeys_;
}

bool TriggerFilter::hasObjectKey( unsigned objectKey ) const
{
  for ( std::vector< unsigned >::const_iterator iO = objectKeys_.begin(); iO != objectKeys_.end(); ++iO ) {
    if ( *iO == objectKey ) {
      return true;
    }
  }
  return false;
}

std::vector< unsigned > TriggerFilter::objectIds() const
{
  return objectIds_;
}

bool TriggerFilter::hasObjectId( unsigned objectId ) const
{
  for ( std::vector< unsigned >::const_iterator iO = objectIds_.begin(); iO != objectIds_.end(); ++iO ) {
    if ( *iO == objectId ) {
      return true;
    }
  }
  return false;
}

int TriggerFilter::status() const
{
  return status_;
}
