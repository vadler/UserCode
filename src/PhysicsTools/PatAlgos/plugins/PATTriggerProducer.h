#ifndef PhysicsTools_PatAlgos_PATTriggerProducer_h
#define PhysicsTools_PatAlgos_PATTriggerProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerProducer
//
// $Id: PATTriggerProducer.h,v 1.5.4.1 2010/02/25 23:52:17 vadler Exp $
//
/**
  \class    pat::PATTriggerProducer PATTriggerProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"
  \brief    Produces the pat::TriggerPathCollection, pat::TriggerFilterCollection and pat::TriggerObjectCollection in PAT layer 0.

   [...]

  \author   Volker Adler
  \version  $Id: PATTriggerProducer.h,v 1.5.4.1 2010/02/25 23:52:17 vadler Exp $
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include <string>

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"


namespace pat {

  class PATTriggerProducer : public edm::EDProducer {

    public:

      explicit PATTriggerProducer( const edm::ParameterSet & iConfig );
      ~PATTriggerProducer() {};

    private:

      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );

      bool onlyStandAlone_;
      // L1
      edm::InputTag tagL1ExtraMu_;
      edm::InputTag tagL1ExtraNoIsoEG_;
      edm::InputTag tagL1ExtraIsoEG_;
      edm::InputTag tagL1ExtraCenJet_;
      edm::InputTag tagL1ExtraForJet_;
      edm::InputTag tagL1ExtraTauJet_;
      edm::InputTag tagL1ExtraETM_;
      edm::InputTag tagL1ExtraHTM_;
      // HLT
      HLTConfigProvider hltConfig_;
      std::string       nameProcess_;
      edm::InputTag     tagTriggerResults_;
      edm::InputTag     tagTriggerEvent_;
      bool              addPathModuleLabels_;

  };

}


#endif
