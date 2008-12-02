#ifndef PhysicsTools_PatAlgos_PATTriggerProducer_h
#define PhysicsTools_PatAlgos_PATTriggerProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerProducer
//
// $Id$
//
/**
  \class    pat::PATTriggerProducer PATTriggerProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"
  \brief    Produces the pat::TriggerEvent and related collections.

   [...]

  \author   Volker Adler
  \version  $Id$
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include <string>

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"


namespace pat {

  class PATTriggerProducer : public edm::EDProducer {

    public:

      explicit PATTriggerProducer( const edm::ParameterSet & iConfig );
      ~PATTriggerProducer();

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
