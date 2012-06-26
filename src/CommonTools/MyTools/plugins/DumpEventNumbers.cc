#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <fstream>

class DumpEventNumbers : public edm::EDAnalyzer {
  public:
    explicit DumpEventNumbers( const edm::ParameterSet & iConfig );
    virtual ~DumpEventNumbers() { if ( outFile_.is_open() ) outFile_.close(); };
    virtual void beginJob();
    virtual void analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup );
    virtual void endJob();
  private:
    std::string fileName_;
    std::ofstream outFile_;
};
DumpEventNumbers::DumpEventNumbers( const edm::ParameterSet & iConfig )
: fileName_( iConfig.getParameter< std::string >( "fileName" ) ) {}
void DumpEventNumbers::beginJob() {
  outFile_.open( fileName_.c_str() );
}
void DumpEventNumbers::analyze( const edm::Event & iEvent, const edm::EventSetup & iSetup ) {
  outFile_ << iEvent.id() << std::endl;
}
void DumpEventNumbers::endJob() {
  outFile_.close();
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( DumpEventNumbers );
