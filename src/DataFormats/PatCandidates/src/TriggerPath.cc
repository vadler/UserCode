//
// $Id:$
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

/// path related

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

/// objects related

/// filters related

pat::TriggerFilterCollection TriggerPath::getFilters() // this certainly has to be checked, protection added
{
  pat::TriggerFilterCollection filters;
  for ( pat::TriggerFilterRefVector::iterator i = filters_.begin(); i != filters_.end(); i++ ) {
    filters.push_back( **i );
  }
  return filters;
}

pat::TriggerFilterCollection TriggerPath::getFiltersFinal()
{
  pat::TriggerFilterCollection dummy; // dummy
  return dummy;
}

pat::TriggerFilter TriggerPath::getFilter( std::string & filterName )
{
  pat::TriggerFilter dummy; // dummy
  return dummy;
}

// pat::TriggerFilter TriggerPath::getFilter( edm::InputTag & filterTag )
// {
//   return getFilter( filterTag.label() );
// }

pat::TriggerFilter TriggerPath::getFilterFailed() // this certainly has to be checked, protection added
{
  pat::TriggerFilter filter;
  for ( pat::TriggerFilterRefVector::iterator i = filters_.begin(); i != filters_.end(); i++ ) {
    filter = **i;
    if ( ! filter.wasAccept() ) return filter;
  }
  return filter;
}

unsigned int TriggerPath::nFilters()
{
  return filters_.size();
}

unsigned int TriggerPath::nFiltersFinal()
{
  return getFiltersFinal().size();
}

/// event related
