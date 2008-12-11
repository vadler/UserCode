//
// $Id$
//


#include "DataFormats/PatCandidates/interface/TriggerEvent.h"


using namespace pat;

/// default constructor
TriggerEvent::TriggerEvent() :
  nameHltTable_()
{
}

/// constructor from values
TriggerEvent::TriggerEvent( std::string & nameHltTable, bool run, bool accept, bool error ) :
  nameHltTable_( nameHltTable ),
  run_( run ),
  accept_( accept ),
  error_( error )
{
}

/// destructor
TriggerEvent::~TriggerEvent()
{
}

/// event related

void TriggerEvent::setNameHltTable( std::string & nameHltTable )
{
  nameHltTable_ = nameHltTable;
}

void TriggerEvent::setRun( bool run )
{
  run_ = run;
}

void TriggerEvent::setAccept( bool accept )
{
  accept_ = accept;
}

void TriggerEvent::setError( bool error )
{
  error_ = error;
}

std::string TriggerEvent::nameHltTable() const
{
  return nameHltTable_;
}

bool TriggerEvent::wasRun() const
{
  return run_;
}

bool TriggerEvent::wasAccept() const
{
  return accept_;
}

bool TriggerEvent::wasError() const
{
  return error_;
}

/// paths related

/// filters related

/// objects related
