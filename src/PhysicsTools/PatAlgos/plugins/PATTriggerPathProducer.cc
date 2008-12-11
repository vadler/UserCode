//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATTriggerPathProducer.h"


using namespace pat;

PATTriggerPathProducer::PATTriggerPathProducer( const edm::ParameterSet & iConfig ) :
  nameProcess_( iConfig.getParameter< std::string >( "processName" ) ),
  tagTriggerResults_( iConfig.getParameter< edm::InputTag >( "triggerResults" ) )
{
  if ( tagTriggerResults_.process().empty() ) {
    tagTriggerResults_ = edm::InputTag( tagTriggerResults_.label(), tagTriggerResults_.instance(), nameProcess_ );
  }

  produces< TriggerPathCollection >();
}

PATTriggerPathProducer::~PATTriggerPathProducer()
{
}

void PATTriggerPathProducer::beginRun( edm::Run & iRun, const edm::EventSetup & iSetup )
{
  if ( ! hltConfig_.init( nameProcess_ ) ) {
    edm::LogError( "errorHltConfigExtraction" ) << "HLT config extraction error with process name " << nameProcess_;
    return;
  }                          
}

void PATTriggerPathProducer::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  if ( hltConfig_.size() <= 0 ) {
    edm::LogError( "errorHltConfigSize" ) << "HLT config size error" << "\n"
                                          << "Check for occurence of an \"errorHltConfigExtraction\" from beginRun()";
    return;
  }
  edm::Handle< edm::TriggerResults > handleTriggerResults;
  iEvent.getByLabel( tagTriggerResults_, handleTriggerResults );
  if ( ! handleTriggerResults.isValid() ) {
    edm::LogError( "errorTriggerResultsValid" ) << "edm::TriggerResults product with InputTag " << tagTriggerResults_.encode() << " not in event";
    return;
  }
  const unsigned int sizePaths( hltConfig_.size() );

  // produce trigger paths
  
  std::auto_ptr< TriggerPathCollection > triggerPaths( new TriggerPathCollection() );
  triggerPaths->reserve( sizePaths );

  for ( unsigned int iP = 0; iP < sizePaths; ++iP ) {
    std::string namePath( hltConfig_.triggerName( iP ) );
    unsigned int indexPath( hltConfig_.triggerIndex( namePath ) );
    TriggerPath triggerPath( namePath, 1, handleTriggerResults->wasrun( indexPath ), handleTriggerResults->accept( indexPath ), handleTriggerResults->error( indexPath ), handleTriggerResults->index( indexPath );
    
    triggerPaths->push_back( triggerPath );
  }
  
  iEvent.put( triggerPaths );
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATTriggerPathProducer );
