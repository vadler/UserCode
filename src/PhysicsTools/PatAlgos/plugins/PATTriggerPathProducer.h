#ifndef PhysicsTools_PatAlgos_PATTriggerPathProducer_h
#define PhysicsTools_PatAlgos_PATTriggerPathProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerPathProducer
//
// $Id$
//
/**
  \class    pat::PATTriggerPathProducer PATTriggerPathProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerPathProducer.h"
  \brief    Produces the pat::TriggerPathCollection in PAT layer 0.

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

#include "DataFormats/PatCandidates/interface/TriggerPath.h"


namespace pat {

  class PATTriggerPathProducer : public edm::EDProducer {

    public:

      explicit PATTriggerPathProducer( const edm::ParameterSet & iConfig );
      ~PATTriggerPathProducer();

    private:

      virtual void beginRun( edm::Run & iRun, const edm::EventSetup & iSetup );
      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );
      
      HLTConfigProvider hltConfig_;
      std::string       nameProcess_;
      edm::InputTag     tagTriggerResults_;

  };

}


#endif
