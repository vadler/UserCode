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

    public:

      /// Constructors and Desctructor

      /// Default constructor
      TriggerCondition();

      /// Destructor
      virtual ~TriggerCondition() {};

      /// Methods

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
