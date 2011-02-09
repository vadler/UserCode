//
// $Id: TriggerAlgorithm.cc,v 1.2 2010/12/16 18:39:17 vadler Exp $
//


#include "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"


using namespace pat;


// Constructors and Destructor


// Default constructor
TriggerAlgorithm::TriggerAlgorithm() :
  name_(),
  alias_(),
  tech_(),
  bit_(),
  prescale_(),
  mask_(),
  decisionBeforeMask_(),
  decisionAfterMask_()
{
  objectKeys_.clear();
}


// Constructor from algorithm name only
TriggerAlgorithm::TriggerAlgorithm( const std::string & name ) :
  name_( name ),
  alias_(),
  tech_(),
  bit_(),
  prescale_(),
  mask_(),
  decisionBeforeMask_(),
  decisionAfterMask_()
{
  objectKeys_.clear();
}


// Constructor from values
TriggerAlgorithm::TriggerAlgorithm( const std::string & name, const std::string & alias, bool tech, unsigned bit, unsigned prescale, bool mask, bool decisionBeforeMask, bool decisionAfterMask ) :
  name_( name ),
  alias_( alias),
  tech_( tech ),
  bit_( bit ),
  prescale_( prescale ),
  mask_( mask ),
  decisionBeforeMask_( decisionBeforeMask ),
  decisionAfterMask_( decisionAfterMask )
{
  objectKeys_.clear();
}


// Methods


// Checks, if a certain trigger object collection index is assigned
bool TriggerAlgorithm::hasObjectKey( unsigned objectKey ) const
{
  for ( size_t iO = 0; iO < objectKeys().size(); ++iO ) {
    if ( objectKeys().at( iO ) == objectKey ) {
      return true;
    }
  }
  return false;
}
