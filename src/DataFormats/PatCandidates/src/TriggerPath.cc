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

/// getters

// returns size of modules_ if name unknown
unsigned TriggerPath::indexModule( const std::string & name ) const
{
  unsigned i( 0 );
  while ( i < modules().size() ) {
    if ( name == modules().at( i ) ) {
      return i;
    }
  }
  return i;
}
