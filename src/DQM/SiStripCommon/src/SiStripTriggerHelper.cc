//
// $Id$
//


#include "DQM/SiStripCommon/interface/SiStripTriggerHelper.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream> // DEBUG


using namespace std;
using namespace edm;


/// Was this event accepted by the configured HLT paths combination?
bool SiStripTriggerHelper::accept( const Event & event, const ParameterSet & config )
{

  /// Configuration parameters
  const string hltInputTagConfig( "hltInputTag" );
  const string hltPathConfig( "hltPaths" );
  const string andOrConfig( "andOr" );
  const string errorReplyConfig( "errorReply" );

  /// Getting the desired reply in case of error from the configuration
  errorReply_ = config.getParameter< bool >( errorReplyConfig );

  /// Getting the TriggerResults InputTag from the configuration
  // If an according InputTag is found in the configuration, it is expected to be meaningful and correct.
  if ( ! config.exists( hltInputTagConfig ) ) return errorReply_;
  hltInputTag_ = config.getParameter< InputTag >( hltInputTagConfig );
  // The process name has to be given.
  if ( hltInputTag_.process().size() == 0 ) {
    LogError( "hltProcess" ) << "HLT TriggerResults InputTag " << hltInputTag_.encode() << " specifies no process";
    return errorReply_;
  }

  /// Getting the HLT path name of question from the configuration
  // An empty configuration parameter is accepted and ignored.
  hltPathNames_.clear();
  if ( config.exists( hltPathConfig ) ) hltPathNames_ = config.getParameter< vector< string > >( hltPathConfig );
  if ( hltPathNames_.empty() ) return errorReply_;
  andOr_ = config.getParameter< bool >( andOrConfig );

  /// Accessing the TriggerResults
  event.getByLabel( hltInputTag_, hltTriggerResults_ );
  if ( ! hltTriggerResults_.isValid() ) {
    LogError( "triggerResultsValid" ) << "TriggerResults product with InputTag " << hltInputTag_.encode() << " not in event";
    return errorReply_;
  }

  /// Getting the HLT configuartion from the provenance
  bool changed( true );
  if ( ! hltConfig_.init( event, hltInputTag_.process(), changed ) ) {
    LogError( "hltConfigInit" ) << "HLT config initialization error with process name " << hltInputTag_.process();
    return errorReply_;
  }
  if ( hltConfig_.size() <= 0 ) {
    LogError( "hltConfigSize" ) << "HLT config size error";
    return errorReply_;
  }

  /// Determine acceptance of HLT path combination and return
  if ( andOr_ ) { // OR combiination
    for ( vector< string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
      if ( acceptPath( *pathName ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
    if ( ! acceptPath( *pathName ) ) return false;
  }
  return true;

}

/// Was this event accepted by this particular HLT path?
bool SiStripTriggerHelper::acceptPath( const string & hltPathName ) const
{

  const unsigned indexPath( hltConfig_.triggerIndex( hltPathName ) );
  if ( indexPath == hltConfig_.size() ) {
    LogError( "hltPathName" ) << "Path " << hltPathName << " is not found in process " << hltInputTag_.process();
    return errorReply_;
  }
  if ( hltTriggerResults_->error( indexPath ) ) {
    LogError( "hltPathError" ) << "Path " << hltPathName << " in error";
    return errorReply_;
  }
  cout << "SiStripTriggerHelper: path " << hltPathName << "->" << hltTriggerResults_->accept( indexPath ) << endl; // DEBUG
  return hltTriggerResults_->accept( indexPath );

}
