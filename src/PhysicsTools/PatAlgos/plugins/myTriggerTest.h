#ifndef PhysicsTools_PatAlgos_myTriggerTest_h
#define PhysicsTools_PatAlgos_myTriggerTest_h


// -*- C++ -*-
//
// Package:    PatAlgos
// Class:      pat::myTriggerTest
// 
// $Id: myTriggerTest.h,v 1.1.2.4 2009/03/13 12:10:36 vadler Exp $
//
/**
  \class myTriggerTest myTriggerTest.h "PhysicsTools/myTriggerTest/plugins/myTriggerTest.h"
  \brief

   [...]

  \author   Volker Adler
  \version  $Id: myTriggerTest.h,v 1.1.2.4 2009/03/13 12:10:36 vadler Exp $
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include <memory>
#include <string>
#include <map>

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include "DataFormats/PatCandidates/interface/TriggerObject.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/TriggerFilter.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "PhysicsTools/UtilAlgos/interface/TFileService.h"
#include "TH1D.h"
#include "TH2D.h"


namespace pat {

  class myTriggerTest : public edm::EDAnalyzer {
  
    public:
    
      explicit myTriggerTest( const edm::ParameterSet & iConfig );
      ~myTriggerTest();
    
    private:
  
      virtual void beginJob( const edm::EventSetup & iSetup ) ;
      virtual void beginRun( edm::Run & iRun, const edm::EventSetup & iSetup );
      virtual void analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup );
      virtual void endRun();
      virtual void endJob();
  
      HLTConfigProvider hltConfig_;
      std::string       nameHLTProcess_;
      edm::InputTag     tagTriggerResults_;
      edm::InputTag     tagTriggerEvent_;
      std::string       namePATProcess_;
      edm::InputTag     tagPatTrigger_;
      edm::InputTag     tagPatTriggerEvent_;
      bool              testPathModuleTags_;
      bool              displayNumbers_;
      bool              displayObjects_;
      bool              displayObjectsStandAlone_;
      bool              displayFilters_;
      bool              displayPaths_;
      bool              displayEvent_;
      bool              displayMatches_;
           
      std::map< std::string, TH1D* > histos1D_;
      std::map< std::string, TH2D* > histos2D_;
  };

}


#endif
