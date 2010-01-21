

#include "DQM/SiStripCommon/interface/SiStripTriggerHelper.h"

#include <iostream> // DEBUG

#include "FWCore/MessageLogger/interface/MessageLogger.h"


bool SiStripTriggerHelper::accept( const edm::Event & event, const edm::ParameterSet & config )
{

  /// Configuration parameters
  const std::string hltInputTagConfig( "hltInputTag" );
  const std::string hltPathConfig( "hltPaths" );
  const std::string andOrConfig( "andOr" );

  /// Getting the TriggerResults InputTag from the configuration
  // If an according InputTag is found in the configuration, it is expected to be meaningful and correct.
  if ( ! config.exists( hltInputTagConfig ) ) return true;
  hltInputTag_ = config.getParameter< edm::InputTag >( hltInputTagConfig );
  // The process name must has to be given.
  if ( hltInputTag_.process().size() == 0 ) {
    edm::LogError( "hltProcess" ) << "HLT TriggerResults InputTag " << hltInputTag_.encode() << " specifies no process";
    return true;
  }

  /// Getting the HLT path name of question from the configuration
  // An empty configuration parameter is accepted and ignored.
  hltPathNames_.clear();
  if ( config.exists( hltPathConfig ) ) hltPathNames_ = config.getParameter< std::vector< std::string > >( hltPathConfig );
  if ( hltPathNames_.empty() ) return true;
  andOr_ = config.getParameter< bool >( andOrConfig );

  /// Accessing the TriggerResults
  event.getByLabel( hltInputTag_, hltTriggerResults_ );
  if ( ! hltTriggerResults_.isValid() ) {
    edm::LogError( "triggerResultsValid" ) << "edm::TriggerResults product with InputTag " << hltInputTag_.encode() << " not in event";
    return true;
  }

  /// Getting the HLT configuartion from the provenance
  bool changed( true );
  if ( ! hltConfig_.init( event, hltInputTag_.process(), changed ) ) {
    edm::LogError( "hltConfigInit" ) << "HLT config initialization error with process name " << hltInputTag_.process();
    return true;
  }
  if ( hltConfig_.size() <= 0 ) {
    edm::LogError( "hltConfigSize" ) << "HLT config size error";
    return true;
  }

  /// Determine acceptance of HLT paths and return
  if ( ! andOr_ ) {
    for ( std::vector< std::string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
      if ( ! acceptPath( *pathName ) ) return false;
    }
    return true;
  }
  for ( std::vector< std::string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
    if ( acceptPath( *pathName ) ) return true;
  }
  return false;

}

/// Checking one particular HLT path
bool SiStripTriggerHelper::acceptPath( const std::string & hltPathName ) const
{

  const unsigned indexPath( hltConfig_.triggerIndex( hltPathName ) );
  if ( indexPath == hltConfig_.size() ) {
    edm::LogError( "hltPathName" ) << "Path " << hltPathName << " is not found in process " << hltInputTag_.process();
    return true;
  }
  if ( hltTriggerResults_->error( indexPath ) ) {
    edm::LogError( "hltPathError" ) << "Path " << hltPathName << " in error";
    return true;
  }
  std::cout << "SiStripTriggerHelper: path " << hltPathName << "->" << hltTriggerResults_->accept( indexPath ) << std::endl; // DEBUG
  return hltTriggerResults_->accept( indexPath );

}
