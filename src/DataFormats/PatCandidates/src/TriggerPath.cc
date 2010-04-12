//
// $Id: TriggerPath.cc,v 1.3 2009/04/27 20:45:18 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerPath.h"


using namespace pat;


/// default constructor

TriggerPath::TriggerPath() :
  name_(),
  index_(),
  prescale_(),
  run_(),
  accept_(),
  error_(),
  lastActiveFilterSlot_()
{
  modules_.clear();
  filterIndices_.clear();
}

/// constructor from values

TriggerPath::TriggerPath( const std::string & name ) :
  name_( name ),
  index_(),
  prescale_(),
  run_(),
  accept_(),
  error_(),
  lastActiveFilterSlot_()
{
  modules_.clear();
  filterIndices_.clear();
}

TriggerPath::TriggerPath( const std::string & name, unsigned index, unsigned prescale, bool run, bool accept, bool error, unsigned lastActiveFilterSlot ) :
  name_( name ),
  index_( index ),
  prescale_( prescale ),
  run_( run ),
  accept_( accept ),
  error_( error ),
  lastActiveFilterSlot_( lastActiveFilterSlot )
{
  modules_.clear();
  filterIndices_.clear();
}

/// getters

// returns size of modules_ if name unknown and -1 if 'modules_' not filled
int TriggerPath::indexModule( const std::string & name ) const
{
  size_t i( 0 );
  while ( i < modules().size() ) {
    if ( name == modules().at( i ) ) {
      return ( int )i;
    }
    ++i;
  }
  return i == 0 ? -1 : ( int )i;
}
