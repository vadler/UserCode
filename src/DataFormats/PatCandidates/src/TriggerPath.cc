//
// $Id: TriggerPath.cc,v 1.5 2010/10/28 19:16:55 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerPath.h"


using namespace pat;


// Constructors and Destructor


// Default constructor
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


// Constructor from path name only
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


// Constructor from values
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


// Methods


// Get the index of a certain module
int TriggerPath::indexModule( const std::string & name ) const
{
  if ( modules_.begin() == modules_.end() ) return -1;
  return ( std::find( modules_.begin(), modules_.end(), name ) - modules_.begin() );
}


// Get names of all L1 seeds with a certain decision
std::vector< std::string > TriggerPath::l1Seeds( const bool decision ) const
{

  std::vector< std::string > seeds;
  for ( L1SeedCollection::const_iterator iSeed = l1Seeds().begin(); iSeed != l1Seeds().end(); ++iSeed ) {
    if ( iSeed->first == decision ) seeds.push_back( iSeed->second );
  }
  return seeds;

}
