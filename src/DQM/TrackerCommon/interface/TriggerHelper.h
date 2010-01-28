#ifndef TriggerHelper_H
#define TriggerHelper_H


// -*- C++ -*-
//
// Package:    DQM/TrackerCommon
// Class:      TriggerHelper
//
// $Id: TriggerHelper.h,v 1.1 2010/01/24 13:47:00 vadler Exp $
//
/**
  \class    TriggerHelper TriggerHelper.h "DQM/TrackerCommon/interface/TriggerHelper.h"
  \brief    Provides a code based selection for HLT path combinations in order to have no failing filters in the CMSSW path.

   [...]

  \author   Volker Adler
  \version  $Id: TriggerHelper.h,v 1.1 2010/01/24 13:47:00 vadler Exp $
*/


#include <string>
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"


class TriggerHelper {

    // Data members
    // L1 access
    L1GtUtils l1Gt_;
    // L1 filter configuration parameters
    std::vector< std::string > l1AlgorithmNames_;
    bool errorReplyL1_;
    // HLT configuration
    HLTConfigProvider hltConfig_;
    // HLT filter configuration parameters
    edm::InputTag hltInputTag_;
    edm::Handle< edm::TriggerResults > hltTriggerResults_;
    std::vector< std::string > hltPathNames_;
    bool errorReplyHlt_;

  public:

    // Constructors and destructor
    TriggerHelper();
    ~TriggerHelper() {};

    // Public methods
    bool accept( const edm::Event & event, const edm::EventSetup & setup, const edm::ParameterSet & config ); // L1 + HLT combined
    bool accept( const edm::Event & event, const edm::ParameterSet & config ); // filters for HLT only                                // HLT only (backward compatible with first version)

  private:

    // Private methods

    // L1
    bool acceptL1( const edm::Event & event, const edm::EventSetup & setup, const edm::ParameterSet & config );
    bool acceptL1Algorithm( const edm::Event & event, std::string l1AlgorithmName );

    // HLT
    bool acceptHlt( const edm::Event & event, const edm::ParameterSet & config );
    bool acceptHltPath( std::string hltPathName ) const;

    // DCS

    // Helpers
    bool negate( std::string & word ) const;

};


#endif
