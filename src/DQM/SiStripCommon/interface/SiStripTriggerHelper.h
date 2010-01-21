#ifndef SiStripTriggerHelper_H
#define SiStripTriggerHelper_H

#include <string>
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"


class SiStripTriggerHelper {

    // HLT configuration
    HLTConfigProvider hltConfig_;

    // Configuration parameters
    edm::InputTag hltInputTag_;
    edm::Handle< edm::TriggerResults > hltTriggerResults_;
    std::vector< std::string > hltPathNames_;
    bool andOr_;

  public:

    SiStripTriggerHelper() { hltPathNames_.clear(); };
    ~SiStripTriggerHelper() {};

    bool accept( const edm::Event & event, const edm::ParameterSet & config );
    bool acceptPath( const std::string & hltPathName ) const;

};


#endif
