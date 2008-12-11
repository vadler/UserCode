#ifndef PhysicsTools_PatAlgos_PATTriggerFilterProducer_h
#define PhysicsTools_PatAlgos_PATTriggerFilterProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerFilterProducer
//
// $Id$
//
/**
  \class    pat::PATTriggerFilterProducer PATTriggerFilterProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerFilterProducer.h"
  \brief    Produces the pat::TriggerFilterCollection in PAT layer 0.

   [...]

  \author   Volker Adler
  \version  $Id$
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "DataFormats/PatCandidates/interface/TriggerFilter.h"


namespace pat {

  class PATTriggerFilterProducer : public edm::EDProducer {

    public:

      explicit PATTriggerFilterProducer( const edm::ParameterSet & iConfig );
      ~PATTriggerFilterProducer();

    private:

      virtual void beginRun( edm::Run & iRun, const edm::EventSetup & iSetup );
      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );
      
      HLTConfigProvider hltConfig_;
      std::string       nameProcess_;
      edm::InputTag     tagTriggerResults_;
      edm::InputTag     tagTriggerEvent_;

  };

}


#endif
