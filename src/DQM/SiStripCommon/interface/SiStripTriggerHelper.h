#ifndef SiStripTriggerHelper_H
#define SiStripTriggerHelper_H


// -*- C++ -*-
//
// Package:    DQM/SiStripCommon
// Class:      SiStripTriggerHelper
//
// $Id$
//
/**
  \class    SiStripTriggerHelper SiStripTriggerHelper.h "DQM/SiStripCommon/interface/SiStripTriggerHelper.h"
  \brief    Provides a code based selection for HLT path combinations in order to have no failing filters in the CMSSW path.

   [...]

  \author   Volker Adler
  \version  $Id$
*/


#include <string>
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"


class SiStripTriggerHelper {

    // Data members
    // HLT configuration
    HLTConfigProvider hltConfig_;
    // Configuration parameters
    edm::InputTag hltInputTag_;
    edm::Handle< edm::TriggerResults > hltTriggerResults_;
    std::vector< std::string > hltPathNames_;
    bool andOr_;
    bool errorReply_;

  public:

    // Constructors and destructor
    SiStripTriggerHelper() { hltPathNames_.clear(); };
    ~SiStripTriggerHelper() {};

    // Public methods
    bool accept( const edm::Event & event, const edm::ParameterSet & config );

  private:

    // Private methods
    bool acceptPath( std::string hltPathName ) const;

};


#endif
