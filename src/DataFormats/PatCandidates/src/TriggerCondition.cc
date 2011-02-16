//
// $Id:$
//


#include "DataFormats/PatCandidates/interface/TriggerCondition.h"


using namespace pat;


// Constructors and Destructor


// Default constructor
TriggerCondition::TriggerCondition() :
  name_()
, accept_()
, triggerObjectType_()
{
  objectKeys_.clear();
}


// Constructor from condition name "only"
TriggerCondition::TriggerCondition( const std::string & name, trigger::TriggerObjectType triggerObjectType ) :
  name_( name )
, accept_()
, triggerObjectType_( triggerObjectType )
{
  objectKeys_.clear();
}


// Constructor from values
TriggerCondition::TriggerCondition( const std::string & name, bool accept, trigger::TriggerObjectType triggerObjectType ) :
  name_( name )
, accept_( accept )
, triggerObjectType_( triggerObjectType )
{
  objectKeys_.clear();
}


// Methods


// Checks, if a certain trigger object collection index is assigned
bool TriggerCondition::hasObjectKey( unsigned objectKey ) const
{
  for ( size_t iO = 0; iO < objectKeys().size(); ++iO ) {
    if ( objectKeys().at( iO ) == objectKey ) return true;
  }
  return false;
}
