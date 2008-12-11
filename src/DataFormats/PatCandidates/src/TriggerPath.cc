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
  error_(),
  lastModule_()
{
}

/// constructor from values

TriggerPath::TriggerPath( std::string & name ) :
  name_( name ),
  prescale_(),
  run_(),
  accept_(),
  error_(),
  lastModule_()
{
}

TriggerPath::TriggerPath( std::string & name, unsigned int prescale, bool run, bool accept, bool error, unsigned int lastModule ) :
  name_( name ),
  prescale_( prescale ),
  run_( run ),
  accept_( accept ),
  error_( error ),
  lastModule_( lastModule )
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

void TriggerPath::setLastModule( unsigned int lastModule )
{
  lastModule_ = lastModule;
}

/// getters

std::string TriggerPath::name() const
{
  return name_;
}

unsigned int TriggerPath::prescale() const
{
  return prescale_;
}

bool TriggerPath::wasRun() const
{
  return run_;
}

bool TriggerPath::wasAccept() const
{
  return accept_;
}

bool TriggerPath::wasError() const
{
  return error_;
}

unsigned int TriggerPath::lastModule() const
{
  return lastModule_;
}
