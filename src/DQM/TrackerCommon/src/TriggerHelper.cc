//
// $Id: TriggerHelper.cc,v 1.1 2010/01/24 13:47:00 vadler Exp $
//


#include "DQM/TrackerCommon/interface/TriggerHelper.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <iostream> // DEBUG


using namespace std;
using namespace edm;


TriggerHelper::TriggerHelper()
{

  l1AlgorithmNames_.clear();
  hltTriggerResults_.clear();
  hltPathNames_.clear();
  dcsStatus_.clear();
  dcsPartitions_.clear();

}


bool TriggerHelper::accept( const edm::Event & event, const edm::EventSetup & setup, const edm::ParameterSet & config )
{

  if ( config.getParameter< bool >( "andOr" ) ) return ( acceptL1( event, setup, config ) || acceptHlt( event, config ) || acceptDcs( event, config ) );
  return ( acceptL1( event, setup, config ) && acceptHlt( event, config ) && acceptDcs( event, config ) );

}


bool TriggerHelper::accept( const Event & event, const ParameterSet & config )
{

  if ( config.getParameter< bool >( "andOr" ) ) return ( acceptHlt( event, config ) || acceptDcs( event, config ) );
  return ( acceptHlt( event, config ) && acceptDcs( event, config ) );

}


/// Was this event accepted by the configured L1 algorithms combination?
bool TriggerHelper::acceptL1( const Event & event, const EventSetup & setup, const ParameterSet & config )
{

  // Configuration parameter tags
  const string l1AlgorithmsConfig( "l1Algorithms" );
  const string andOrConfig( "andOrL1" );
  const string errorReplyConfig( "errorReplyL1" );

  // Getting the L1 algorithm names of question from the configuration
  // An empty configuration parameter switches the filter off.
  l1AlgorithmNames_ = config.getParameter< vector< string > >( l1AlgorithmsConfig );
  if ( l1AlgorithmNames_.empty() ) return true;

  // Getting remaining configuration parameters
  errorReplyL1_ = config.getParameter< bool >( errorReplyConfig );

  l1Gt_.retrieveL1EventSetup( setup );

  // Determine acceptance of L1 algorithm combination and return
  if ( config.getParameter< bool >( andOrConfig ) ) { // OR combination
    for ( vector< string >::const_iterator l1Algorithm = l1AlgorithmNames_.begin(); l1Algorithm != l1AlgorithmNames_.end(); ++l1Algorithm ) {
      if ( acceptL1Algorithm( event, *l1Algorithm ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator l1Algorithm = l1AlgorithmNames_.begin(); l1Algorithm != l1AlgorithmNames_.end(); ++l1Algorithm ) {
    if ( ! acceptL1Algorithm( event, *l1Algorithm ) ) return false;
  }
  return true;

}


/// Was this event accepted by this particular L1 algorithm?
bool TriggerHelper::acceptL1Algorithm( const Event & event, string l1AlgorithmName )
{


  // Check empty strings
  if ( l1AlgorithmName.empty() ) {
    LogError( "l1AlgorithmName" ) << "Empty algorithm name";
    return errorReplyL1_;
  }

  // Negated algorithms
  bool notAlgo( negate( l1AlgorithmName ) );
  if ( notAlgo && l1AlgorithmName.empty() ) {
    LogError( "l1AlgorithmName" ) << "Empty (negated) algorithm name";
    return errorReplyL1_;
  }
  const string algoName( l1AlgorithmName );

  // Get L1 decision
  int error( -1 );
  const bool decision( l1Gt_.decision( event, algoName, error ) );

  // Error checks
  if ( error == 1 ) {
    LogError( "l1AlgorithmInMenu" ) << "L1 algorithm " << algoName << " does not exist in the L1 menu";
    return errorReplyL1_;
  } else if ( error != 0 ) {
    LogError( "l1AlgorithmError" ) << "L1 algorithm " << algoName << " received error code " << error << " from L1GtUtils::decisionBeforeMask";
    return errorReplyL1_;
  }

  // Return decision
  cout << "  TriggerHelper: algo "; // DEBUG
  if ( notAlgo ) cout << "~" << algoName << " -> " << ( ! decision ) << endl; // DEBUG
  else           cout        << algoName << " -> " <<     decision   << endl; // DEBUG
  return notAlgo ? ( ! decision ) : decision;

}


/// Was this event accepted by the configured HLT paths combination?
bool TriggerHelper::acceptHlt( const Event & event, const ParameterSet & config )
{

  // Configuration parameter tags
  const string hltInputTagConfig( "hltInputTag" );
  const string hltPathConfig( "hltPaths" );
  const string andOrConfig( "andOrHlt" );
  const string errorReplyConfig( "errorReplyHlt" );

  // Getting the TriggerResults InputTag from the configuration
  // If an according InputTag is found in the configuration, it is expected to be meaningful and correct.
  // If not, the filter is switched off.
  if ( ! config.exists( hltInputTagConfig ) ) return true;
  hltInputTag_ = config.getParameter< InputTag >( hltInputTagConfig );

  // Getting the HLT path names of question from the configuration
  // An empty configuration parameter acts also as switch.
  hltPathNames_ = config.getParameter< vector< string > >( hltPathConfig );
  if ( hltPathNames_.empty() ) return true;

  // Getting remaining configuration parameters
  errorReplyHlt_ = config.getParameter< bool >( errorReplyConfig );

  // Checking the TriggerResults InputTag
  // The process name has to be given.
  if ( hltInputTag_.process().size() == 0 ) {
    LogError( "hltProcess" ) << "HLT TriggerResults InputTag " << hltInputTag_.encode() << " specifies no process";
    return errorReplyHlt_;
  }

  // Accessing the TriggerResults
  event.getByLabel( hltInputTag_, hltTriggerResults_ );
  if ( ! hltTriggerResults_.isValid() ) {
    LogError( "triggerResultsValid" ) << "TriggerResults product with InputTag " << hltInputTag_.encode() << " not in event";
    return errorReplyHlt_;
  }

  // Getting the HLT configuration from the provenance
  bool changed( true );
  if ( ! hltConfig_.init( event, hltInputTag_.process(), changed ) ) {
    LogError( "hltConfigInit" ) << "HLT config initialization error with process name " << hltInputTag_.process();
    return errorReplyHlt_;
  }
  if ( hltConfig_.size() <= 0 ) {
    LogError( "hltConfigSize" ) << "HLT config size error";
    return errorReplyHlt_;
  }

  // Determine acceptance of HLT path combination and return
  if ( config.getParameter< bool >( andOrConfig ) ) { // OR combination
    for ( vector< string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
      if ( acceptHltPath( *pathName ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator pathName = hltPathNames_.begin(); pathName != hltPathNames_.end(); ++pathName ) {
    if ( ! acceptHltPath( *pathName ) ) return false;
  }
  return true;

}


/// Was this event accepted by this particular HLT path?
bool TriggerHelper::acceptHltPath( string hltPathName ) const
{

  // Check empty strings
  if ( hltPathName.empty() ) {
    LogError( "hltPathName" ) << "Empty path name";
    return errorReplyHlt_;
  }

  // Negated paths
  bool notPath( negate( hltPathName ) );
  if ( notPath && hltPathName.empty() ) {
    LogError( "hltPathName" ) << "Empty (negated) path name";
    return errorReplyHlt_;
  }

  // Further error checks
  const unsigned indexPath( hltConfig_.triggerIndex( hltPathName ) );
  if ( indexPath == hltConfig_.size() ) {
    LogError( "hltPathInProcess" ) << "HLT path " << hltPathName << " is not found in process " << hltInputTag_.process();
    return errorReplyHlt_;
  }
  if ( hltTriggerResults_->error( indexPath ) ) {
    LogError( "hltPathError" ) << "HLT path " << hltPathName << " in error";
    return errorReplyHlt_;
  }

  // Determine decision
  cout << "  TriggerHelper: path "; // DEBUG
  if ( notPath ) cout << "~" << hltPathName; // DEBUG
  else           cout        << hltPathName; // DEBUG
  cout << " -> " << hltTriggerResults_->accept( indexPath ) << endl; // DEBUG
  return notPath ? ( ! hltTriggerResults_->accept( indexPath ) ) : hltTriggerResults_->accept( indexPath );

}


bool TriggerHelper::acceptDcs( const edm::Event & event, const edm::ParameterSet & config )
{


  // Configuration parameter tags
  const string dcsInputTagConfig( "dcsInputTag" );
  const string dcsPartitionsConfig( "dcsPartitions" );
  const string andOrConfig( "andOrDcs" );
  const string errorReplyConfig( "errorReplyDcs" );

  // Getting the DcsStatusCollection InputTag from the configuration
  // If an according InputTag is found in the configuration, it is expected to be meaningful and correct.
  // If not, the filter is switched off.
  if ( ! config.exists( dcsInputTagConfig ) ) return true;
  dcsInputTag_ = config.getParameter< InputTag >( dcsInputTagConfig );

  // Getting the DCS partition numbers of question from the configuration
  // An empty configuration parameter acts also as switch.
  dcsPartitions_ = config.getParameter< vector< int > >( dcsPartitionsConfig );
  if ( dcsPartitions_.empty() ) return true;

  // Getting remaining configuration parameters
  errorReplyDcs_ = config.getParameter< bool >( errorReplyConfig );

  // Accessing the DcsStatusCollection
  event.getByLabel( dcsInputTag_, dcsStatus_ );
  if ( ! dcsStatus_.isValid() ) {
    LogError( "dcsStatusValid" ) << "DcsStatusCollection product with InputTag " << dcsInputTag_.encode() << " not in event";
    return errorReplyDcs_;
  }

  // Determine acceptance of DCS partition combination and return
  if ( config.getParameter< bool >( andOrConfig ) ) { // OR combination
    for ( vector< int >::const_iterator partitionNumber = dcsPartitions_.begin(); partitionNumber != dcsPartitions_.end(); ++partitionNumber ) {
      if ( acceptDcsPartition( *partitionNumber ) ) return true;
    }
    return false;
  }
  for ( vector< int >::const_iterator partitionNumber = dcsPartitions_.begin(); partitionNumber != dcsPartitions_.end(); ++partitionNumber ) {
    if ( ! acceptDcsPartition( *partitionNumber ) ) return false;
  }
  return true;

}


bool TriggerHelper::acceptDcsPartition( int dcsPartition ) const
{

  // Error checks
  switch( dcsPartition ) {
    case DcsStatus::EBp   :
    case DcsStatus::EBm   :
    case DcsStatus::EEp   :
    case DcsStatus::EEm   :
    case DcsStatus::HBHEa :
    case DcsStatus::HBHEb :
    case DcsStatus::HBHEc :
    case DcsStatus::HF    :
    case DcsStatus::HO    :
    case DcsStatus::RPC   :
    case DcsStatus::DT0   :
    case DcsStatus::DTp   :
    case DcsStatus::DTm   :
    case DcsStatus::CSCp  :
    case DcsStatus::CSCm  :
    case DcsStatus::CASTOR:
    case DcsStatus::TIBTID:
    case DcsStatus::TOB   :
    case DcsStatus::TECp  :
    case DcsStatus::TECm  :
    case DcsStatus::BPIX  :
    case DcsStatus::FPIX  :
    case DcsStatus::ESp   :
    case DcsStatus::ESm   :
      break;
    default:
      LogError( "dcsPartition" ) << "DCS partition number " << dcsPartition << " does not exist";
      return errorReplyDcs_;
  }

  // Determine decision
  cout << "  TriggerHelper: partition " << dcsPartition << " -> " << dcsStatus_->at( 0 ).ready( dcsPartition ) << endl; // DEBUG
  return dcsStatus_->at( 0 ).ready( dcsPartition );

}


/// Checks for negated words
bool TriggerHelper::negate( string & word ) const
{

  bool negate( false );
  if ( word.at( 0 ) == '~' ) {
    negate = true;
    word.erase( 0, 1 );
  }
  return negate;

}
