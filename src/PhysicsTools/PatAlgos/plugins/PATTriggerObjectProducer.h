#ifndef PhysicsTools_PatAlgos_PATTriggerObjectProducer_h
#define PhysicsTools_PatAlgos_PATTriggerObjectProducer_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::PATTriggerObjectProducer
//
// $Id$
//
/**
  \class    pat::PATTriggerObjectProducer PATTriggerObjectProducer.h "PhysicsTools/PatAlgos/plugins/PATTriggerObjectProducer.h"
  \brief    Produces the pat::TriggerObjectCollection in PAT layer 0.

   [...]

  \author   Volker Adler
  \version  $Id$
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include <string>
#include <map>
#include <cassert>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"

#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#include "DataFormats/PatCandidates/interface/TriggerObject.h"


namespace pat {

  class PATTriggerObjectProducer : public edm::EDProducer {

    public:

      explicit PATTriggerObjectProducer( const edm::ParameterSet & iConfig );
      ~PATTriggerObjectProducer();

    private:

      virtual void produce( edm::Event & iEvent, const edm::EventSetup & iSetup );
      
      std::string   nameProcess_;
      edm::InputTag tagTriggerEvent_;

  };

}


#endif
