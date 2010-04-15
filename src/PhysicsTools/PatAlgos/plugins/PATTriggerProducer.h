#ifndef PhysicsTools_PatAlgos_PATTriggerProducer_h
#define PhysicsTools_PatAlgos_PATTriggerProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerProducer
//
// $Id: PATTriggerProducer.h,v 1.8 2010/03/18 23:04:54 vadler Exp $
//
/**
  \class    pat::PATTriggerProducer PATTriggerProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerProducer.h"
  \brief    Produces the pat::TriggerPathCollection, pat::TriggerFilterCollection and pat::TriggerObjectCollection.

   [...]

  \author   Volker Adler
  \version  $Id: PATTriggerProducer.h,v 1.8 2010/03/18 23:04:54 vadler Exp $
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

      virtual void beginRun( edm::Run & iRun, const edm::EventSetup & iSetup );
      virtual void beginLuminosityBlock( edm::LuminosityBlock & iLuminosityBlock, const edm::EventSetup & iSetup );
      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );

      bool onlyStandAlone_; // configuration
      // L1
      edm::InputTag tagL1ExtraMu_;      // configuration (optional)
      edm::InputTag tagL1ExtraNoIsoEG_; // configuration (optional)
      edm::InputTag tagL1ExtraIsoEG_;   // configuration (optional)
      edm::InputTag tagL1ExtraCenJet_;  // configuration (optional)
      edm::InputTag tagL1ExtraForJet_;  // configuration (optional)
      edm::InputTag tagL1ExtraTauJet_;  // configuration (optional)
      edm::InputTag tagL1ExtraETM_;     // configuration (optional)
      edm::InputTag tagL1ExtraHTM_;     // configuration (optional)
      // HLT
      HLTConfigProvider         hltConfig_;
      bool                      hltConfigInit_;
      std::string               nameProcess_;           // configuration
      edm::InputTag             tagTriggerResults_;     // configuration (optional with default)
      edm::InputTag             tagTriggerEvent_;       // configuration (optional with default)
      std::string               hltPrescaleLabel_;      // configuration (optional)
      std::string               labelHltPrescaleTable_; // configuration (optional)
      trigger::HLTPrescaleTable hltPrescaleTableRun_;
      trigger::HLTPrescaleTable hltPrescaleTableLumi_;
      bool                      addPathModuleLabels_;   // configuration (optional with default)

  };

}


#endif
