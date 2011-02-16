#ifndef DataFormats_PatCandidates_TriggerCondition_h
#define DataFormats_PatCandidates_TriggerCondition_h


// -*- C++ -*-
//
// Package:    PatCandidates
// Class:      pat::TriggerCondition
//
// $Id:$
//
/**
  \class    pat::TriggerCondition TriggerCondition.h "DataFormats/PatCandidates/interface/TriggerCondition.h"
  \brief    Analysis-level L1 trigger condition class

   TriggerCondition implements a container for trigger conditions' information within the 'pat' namespace.
   For detailed information, consult
   https://twiki.cern.ch/twiki/bin/view/CMS/SWGuidePATTrigger#TriggerCondition

  \author   Volker Adler
  \version  $Id:$
*/


#include <string>
#include <vector>

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/Common/interface/RefProd.h"
#include "DataFormats/Common/interface/RefVector.h"
#include "DataFormats/Common/interface/RefVectorIterator.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

namespace pat {

  class TriggerCondition {

      /// Data Members

      /// Name of the condition
      std::string name_;
      /// Did condition succeed?
      bool accept_;
      /// Special identifier for the used trigger object type as defined in
      /// trigger::TriggerObjectType (DataFormats/HLTReco/interface/TriggerTypeDefs.h)
      trigger::TriggerObjectType triggerObjectType_;
      /// Indeces of trigger objects in pat::TriggerObjectCollection in event
      /// as produced together with the pat::TriggerAlgorithmCollection
      std::vector< unsigned > objectKeys_;

    public:

      /// Constructors and Desctructor

      /// Default constructor
      TriggerCondition();
      /// Constructor from condition name "only"
      TriggerCondition( const std::string & name, trigger::TriggerObjectType triggerObjectType = trigger::TriggerObjectType( 0 ) );
      /// Constructor from values
      TriggerCondition( const std::string & name, bool accept, trigger::TriggerObjectType triggerObjectType = trigger::TriggerObjectType( 0 ) );

      /// Destructor
      virtual ~TriggerCondition() {};

      /// Methods

      /// Set the condition name
      void setName( const std::string & name ) { name_ = name; };
      /// Set the success flag
      void setAccept( bool accept ) { accept_ = accept; };
      /// Set the trigger object type
      void setTriggerObjectType( trigger::TriggerObjectType triggerObjectType ) { triggerObjectType_ = triggerObjectType; };
      void setTriggerObjectType( int triggerObjectType )                        { triggerObjectType_ = trigger::TriggerObjectType( triggerObjectType ); };
      /// Add a new trigger object collection index
      void addObjectKey( unsigned objectKey ) { if ( ! hasObjectKey( objectKey ) ) objectKeys_.push_back( objectKey ); };
      /// Get the filter label
      std::string name() const { return name_; };
      /// Get the success flag
      bool wasAccept() const { return accept_; };
      /// Get the trigger object type
      int triggerObjectType() const { return triggerObjectType_; };
      /// Get all trigger object collection indeces
      std::vector< unsigned > objectKeys() const { return objectKeys_; };
      /// Checks, if a certain trigger object collection index is assigned
      bool hasObjectKey( unsigned objectKey ) const;

  };


  /// Collection of TriggerCondition
  typedef std::vector< TriggerCondition >                      TriggerConditionCollection;
  /// Persistent reference to an item in a TriggerConditionCollection
  typedef edm::Ref< TriggerConditionCollection >               TriggerConditionRef;
  /// Persistent reference to a TriggerConditionCollection product
  typedef edm::RefProd< TriggerConditionCollection >           TriggerConditionRefProd;
  /// Vector of persistent references to items in the same TriggerConditionCollection
  typedef edm::RefVector< TriggerConditionCollection >         TriggerConditionRefVector;
  /// Const iterator over vector of persistent references to items in the same TriggerConditionCollection
  typedef edm::RefVectorIterator< TriggerConditionCollection > TriggerConditionRefVectorIterator;

}


#endif
