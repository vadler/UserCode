//
// $Id$
//


#include "DataFormats/PatCandidates/interface/TriggerPath.h"


using namespace pat;

/// default constructor

TriggerPath::TriggerPath() :
  name_(),
  prescale_(),
  run_(),
  accept_(),
  error_()
{
}

/// constructor from values

TriggerPath::TriggerPath( std::string & name ) :
  name_( name ),
  prescale_(),
  run_(),
  accept_(),
  error_()
{
}

TriggerPath::TriggerPath( std::string & name, unsigned int prescale, bool run, bool accept, bool error) :
  name_( name ),
  prescale_( prescale ),
  run_( run ),
  accept_( accept ),
  error_( error )
{
}

/// destructor

TriggerPath::~TriggerPath()
{
}

/// setters

void TriggerPath::setName( std::string & name )
{
  name_ = name;
}

void TriggerPath::setPrescale( unsigned int prescale )
{
  prescale_ = prescale;
}

void TriggerPath::setRun( bool run )
{
  run_ = run;
}

void TriggerPath::setAccept( bool accept )
{
  accept_ = accept;
}

void TriggerPath::setError( bool error )
{
  error_ = error;
}

/// getters

std::string TriggerPath::getName()
{
  return name_;
}

unsigned int TriggerPath::getPrescale()
{
  return prescale_;
}

bool TriggerPath::wasRun()
{
  return run_;
}

bool TriggerPath::wasAccept()
{
  return accept_;
}

bool TriggerPath::wasError()
{
  return error_;
}
