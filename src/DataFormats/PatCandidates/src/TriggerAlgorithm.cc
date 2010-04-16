//
// $Id$
//


#include "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"


using namespace pat;


/// default constructor

TriggerAlgorithm::TriggerAlgorithm() :
  name_(),
  tech_(),
  bit_(),
  prescale_(),
  run_(),
  accept_(),
  error_()
{}

/// constructor from values

TriggerAlgorithm::TriggerAlgorithm( const std::string & name ) :
  name_( name ),
  tech_(),
  bit_(),
  prescale_(),
  run_(),
  accept_(),
  error_()
{}

TriggerAlgorithm::TriggerAlgorithm( const std::string & name, bool tech, unsigned bit, unsigned prescale, bool run, bool accept, bool error ) :
  name_( name ),
  tech_( tech ),
  bit_( bit ),
  prescale_( prescale ),
  run_( run ),
  accept_( accept ),
  error_( error )
{}

/// getters
