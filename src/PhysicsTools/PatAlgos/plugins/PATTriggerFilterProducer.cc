//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerFilterProducer.h"


using namespace pat;

PATTriggerFilterProducer::PATTriggerFilterProducer( const edm::ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerEvent_( iConfig.getParameter< edm::InputTag >( "triggerEvent" ) )
{
  if ( tagTriggerEvent_.process().empty() ) {
    tagTriggerEvent_ = edm::InputTag( tagTriggerEvent_.label(), tagTriggerEvent_.instance(), nameProcess_ );
  }

  produces< TriggerFilterCollection >();
}

PATTriggerFilterProducer::~PATTriggerFilterProducer()
{
}

void PATTriggerFilterProducer::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }                          
}

void PATTriggerFilterProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  if ( hltConfig_.size() <= 0 ) {
    edm::LogError( "errorHltConfigSize" ) << "HLT config size error" << "\n"
                                          << "Check for occurence of an \"errorHltConfigExtraction\" from beginRun()";
    return;
  }
  edm::Handle< trigger::TriggerEvent > handleTriggerEvent;
  iEvent.getByLabel( tagTriggerEvent_, handleTriggerEvent );
  if ( ! handleTriggerEvent.isValid() ) {
    edm::LogError( "errorTriggerEventValid" ) << "trigger::TriggerEvent product with InputTag " << tagTriggerEvent_.encode() << " not in event";
    return;
  }
  const unsigned int sizeFilters( handleTriggerEvent->sizeFilters() );
  
  // produce trigger filters
  // from TriggerEvent, which contains only the "last active filter" (doesn't help for x-channels)
  
  std::auto_ptr< TriggerFilterCollection > triggerFilters( new TriggerFilterCollection() );
  triggerFilters->reserve( sizeFilters );
  
  for ( unsigned int iF = 0; iF < sizeFilters; ++iF ) {
    std::string filterLabel( handleTriggerEvent->filterTag( iF ).label() );
    TriggerFilter triggerFilter( filterLabel );
    std::string filterType( hltConfig_.moduleType( filterLabel ) );
    triggerFilter.setType( filterType );
    triggerFilter.setWasRun( true ); // applies automatically only for "last active filter" from TriggerEvent
//     triggerFilter.setAccept(  ); // to be figured out
    triggerFilters->push_back( triggerFilter );
  }
  
  iEvent.put( triggerFilters );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerFilterProducer );
