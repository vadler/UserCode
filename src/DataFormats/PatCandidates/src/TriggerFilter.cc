//
// $Id:$
//


#include "DataFormats/PatCandidates/interface/TriggerFilter.h"


using namespace pat;

/// default constructor

TriggerFilter::TriggerFilter() :
  label_(),
  labelInput_(),
  objectId_(),
  run_(),
  accept_()
{
}

/// constructor from values

TriggerFilter::TriggerFilter( std::string & label, std::string & type ) :
  label_( label ),
  type_(type),
  labelInput_(),
  objectId_(),
  run_(),
  accept_()
{
}

TriggerFilter::TriggerFilter( edm::InputTag & tag, std::string & type ) :
  label_( tag.label() ),
  type_(type),
  labelInput_(),
  objectId_(),
  run_(),
  accept_()
{
}

/// destructor

TriggerFilter::~TriggerFilter()
{
}

/// filter related

void TriggerFilter::setLabel( std::string & label )
{
  label_ = label;
}

void TriggerFilter::setType( std::string & type )
{
  type_ = type;
}

void TriggerFilter::setLableInput( std::string & labelInput )
{
  labelInput_ = labelInput;
}

void TriggerFilter::setObjectId( unsigned int objectId )
{
  objectId_ = objectId;
}

void TriggerFilter::setWasRun( bool run )
{
  run_ = run;
}

void TriggerFilter::setWasAccept( bool accept )
{
  accept_ = accept;
}

std::string & TriggerFilter::getLabel()
{
  return label_;
}

std::string & TriggerFilter::getType()
{
  return type_;
}

std::string & TriggerFilter::getLabelInput()
{
  return labelInput_;
}

unsigned int  TriggerFilter::getObjectId()
{
  return objectId_;
}

bool          TriggerFilter::wasRun()
{
  return run_;
}

bool          TriggerFilter::wasAccept()
{
  return accept_;
}

/// objects related

pat::TriggerObjectCollection TriggerFilter::getObjects() // this certainly has to be checked, protection added
{
  pat::TriggerObjectCollection objects;
  for ( pat::TriggerObjectRefVector::iterator i = objects_.begin(); i != objects_.end(); i++ ) {
    objects.push_back( **i );
  }
  return objects;
}

unsigned int TriggerFilter::nObjects()
{
  return objects_.size();
}

/// paths related

/// event related
