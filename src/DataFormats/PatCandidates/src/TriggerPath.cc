//
// $Id: TriggerPath.cc,v 1.1.2.1 2008/12/18 11:26:16 vadler Exp $
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
  lastActiveModule_()
{
  modules_.clear();
}

/// constructor from values

TriggerPath::TriggerPath( const std::string & name ) :
  name_( name ),
  prescale_(),
  run_(),
  accept_(),
  error_(),
  lastActiveModule_()
{
  modules_.clear();
}

TriggerPath::TriggerPath( const std::string & name, unsigned prescale, bool run, bool accept, bool error, unsigned lastActiveModule ) :
  name_( name ),
  prescale_( prescale ),
  run_( run ),
  accept_( accept ),
  error_( error ),
  lastActiveModule_( lastActiveModule )
{
  modules_.clear();
}

/// destructor

TriggerPath::~TriggerPath()
{
}

/// setters

void TriggerPath::setName( const std::string & name )
{
  name_ = name;
}

void TriggerPath::setPrescale( unsigned prescale )
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

void TriggerPath::addModule( const std::string & name )
{
  modules_.push_back( name );
}

void TriggerPath::setLastActiveModule( unsigned lastActiveModule )
{
  lastActiveModule_ = lastActiveModule;
}

/// getters

std::string TriggerPath::name() const
{
  return name_;
}

unsigned TriggerPath::prescale() const
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

std::vector< std::string > TriggerPath::modules() const
{
  return modules_;
}

unsigned TriggerPath::indexModule( const std::string & name ) const
{
  unsigned i( 0 );
  while ( i < modules().size() ) {
    if ( name == modules_[ i ] ) {
      return i;
    }
  }
  return i;
}

unsigned TriggerPath::lastActiveModule() const
{
  return lastActiveModule_;
}
