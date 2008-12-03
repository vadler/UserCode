//
// $Id$
//


#include "DataFormats/PatCandidates/interface/TriggerFilter.h"


using namespace pat;

/// default constructor

TriggerFilter::TriggerFilter() :
  label_(),
  type_(),
  labelInput_(),
  objectId_(),
  run_(),
  accept_()
{
}

/// constructor from values

TriggerFilter::TriggerFilter( const std::string & label ) :
  label_( label ),
  type_(),
  labelInput_(),
  objectId_(),
  run_(),
  accept_()
{
}

TriggerFilter::TriggerFilter( const edm::InputTag & tag ) :
  label_( tag.label() ),
  type_(),
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

/// setters

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

/// getters

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
