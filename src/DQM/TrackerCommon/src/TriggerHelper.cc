//
// $Id: TriggerHelper.cc,v 1.5 2010/02/02 22:17:52 vadler Exp $
//


#include "DQM/TrackerCommon/interface/TriggerHelper.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GtLogicParser.h"
#include <iostream> // DEBUG


using namespace std;
using namespace edm;


TriggerHelper::TriggerHelper()
{

  hltTriggerResults_.clear();
  dcsStatus_.clear();

}


/// L1, HLT and DCS filters combined
bool TriggerHelper::accept( const edm::Event & event, const edm::EventSetup & setup, const edm::ParameterSet & config )
{

  // Getting the and/or switch from the configuration
  // If it does not exist, the configuration is considered not to be present,
  // and the filter dos not have any effect.
  if ( ! config.exists( "andOr" ) ) return true;

  // Determine decision
  if ( config.getParameter< bool >( "andOr" ) ) return ( acceptL1( event, setup, config ) || acceptHlt( event, config ) || acceptDcs( event, config ) );
  return ( acceptL1( event, setup, config ) && acceptHlt( event, config ) && acceptDcs( event, config ) );

}


/// HLT and DCS filters only
bool TriggerHelper::accept( const Event & event, const ParameterSet & config )
{

  // Getting the and/or switch from the configuration
  // If it does not exist, the configuration is considered not to be present,
  // and the filter dos not have any effect.
  if ( ! config.exists( "andOr" ) ) return true;

  // Determine decision
  if ( config.getParameter< bool >( "andOr" ) ) return ( acceptHlt( event, config ) || acceptDcs( event, config ) );
  return ( acceptHlt( event, config ) && acceptDcs( event, config ) );

}


/// Was this event accepted by the configured L1 algorithms combination?
bool TriggerHelper::acceptL1( const Event & event, const EventSetup & setup, const ParameterSet & config )
{

  // Getting configuration parameters
  const vector< string > l1AlgorithmNames( config.getParameter< vector< string > >( "l1Algorithms" ) );
  errorReplyL1_ = config.getParameter< bool >( "errorReplyL1" );

  // An empty L1 algorithms list acts as switch.
  if ( l1AlgorithmNames.empty() ) return true;

  // Getting the L1 event setup
  l1Gt_.retrieveL1EventSetup( setup );

  // Determine decision of L1 algorithm combination and return
  if ( config.getParameter< bool >( "andOrL1" ) ) { // OR combination
    for ( vector< string >::const_iterator l1Algorithm = l1AlgorithmNames.begin(); l1Algorithm != l1AlgorithmNames.end(); ++l1Algorithm ) {
      if ( acceptL1Algorithm( event, *l1Algorithm ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator l1Algorithm = l1AlgorithmNames.begin(); l1Algorithm != l1AlgorithmNames.end(); ++l1Algorithm ) {
    if ( ! acceptL1Algorithm( event, *l1Algorithm ) ) return false;
  }
  return true;

}


/// Was this event accepted by this particular L1 algorithm?
bool TriggerHelper::acceptL1Algorithm( const Event & event, string l1LogicalExpression )
{


  // Check empty strings
  if ( l1LogicalExpression.empty() ) {
    LogError( "l1AlgorithmName" ) << "Empty algorithm name";
    return errorReplyL1_;
  }

  // Negated algorithms
  bool notAlgo( negate( l1LogicalExpression ) );
  if ( notAlgo && l1LogicalExpression.empty() ) {
    LogError( "l1AlgorithmName" ) << "Empty (negated) algorithm name";
    return errorReplyL1_;
  }

  // Parse logical expression and determine L1 decision
  L1GtLogicParser l1AlgoLogicParser( l1LogicalExpression );
  // Loop over tokens
  for ( size_t iToken = 0; iToken < l1AlgoLogicParser.operandTokenVector().size(); ++iToken ) {
    int error( -1 );
    const string algoName( l1AlgoLogicParser.operandTokenVector().at( iToken ).tokenName );
    const bool decision( l1Gt_.decision( event, algoName, error ) );
    cout << "  TriggerHelper: algo name " << algoName << " -> " << decision << endl; // DEBUG
    // Error checks
    if ( error == 1 ) {
      LogError( "l1AlgorithmInMenu" ) << "L1 algorithm " << algoName << " does not exist in the L1 menu";
      return errorReplyL1_;
    } else if ( error != 0 ) {
      LogError( "l1AlgorithmError" ) << "L1 algorithm " << algoName << " received error code " << error << " from L1GtUtils::decisionBeforeMask";
      return errorReplyL1_;
    }
    // Manipulate algo decision as stored in the parser
    l1AlgoLogicParser.operandTokenVector().at( iToken ).tokenResult = decision;
  }

  // Return decision
  const bool l1Decision( l1AlgoLogicParser.expressionResult() );
  cout << "  TriggerHelper: logical expression "; // DEBUG
  if ( notAlgo ) cout << "~(" << l1LogicalExpression << ") -> " << ( ! l1Decision ) << endl; // DEBUG
  else           cout         << l1LogicalExpression << " -> "  <<     l1Decision   << endl; // DEBUG
  return notAlgo ? ( ! l1Decision ) : l1Decision;

}


/// Was this event accepted by the configured HLT paths combination?
bool TriggerHelper::acceptHlt( const Event & event, const ParameterSet & config )
{

  // Getting configuration parameters
  const vector< string > hltPathNames( config.getParameter< vector< string > >( "hltPaths" ) );
  hltInputTag_   = config.getParameter< InputTag >( "hltInputTag" );
  errorReplyHlt_ = config.getParameter< bool >( "errorReplyHlt" );

  // An empty HLT paths list acts as switch.
  if ( hltPathNames.empty() ) return true;

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

  // Determine decision of HLT path combination and return
  if ( config.getParameter< bool >( "andOrHlt" ) ) { // OR combination
    for ( vector< string >::const_iterator pathName = hltPathNames.begin(); pathName != hltPathNames.end(); ++pathName ) {
      if ( acceptHltPath( *pathName ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator pathName = hltPathNames.begin(); pathName != hltPathNames.end(); ++pathName ) {
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

  // Getting configuration parameters
  const InputTag dcsInputTag( config.getParameter< InputTag >( "dcsInputTag" ) );
  const vector< int > dcsPartitions( config.getParameter< vector< int > >( "dcsPartitions" ) );
  errorReplyDcs_ = config.getParameter< bool >( "errorReplyDcs" );

  // An empty DCS partitions list acts as switch.
  if ( dcsPartitions.empty() ) return true;

  // Accessing the DcsStatusCollection
  event.getByLabel( dcsInputTag, dcsStatus_ );
  if ( ! dcsStatus_.isValid() ) {
    LogError( "dcsStatusValid" ) << "DcsStatusCollection product with InputTag " << dcsInputTag.encode() << " not in event";
    return errorReplyDcs_;
  }

  // Determine decision of DCS partition combination and return
  if ( config.getParameter< bool >( "andOrDcs" ) ) { // OR combination
    for ( vector< int >::const_iterator partitionNumber = dcsPartitions.begin(); partitionNumber != dcsPartitions.end(); ++partitionNumber ) {
      if ( acceptDcsPartition( *partitionNumber ) ) return true;
    }
    return false;
  }
  for ( vector< int >::const_iterator partitionNumber = dcsPartitions.begin(); partitionNumber != dcsPartitions.end(); ++partitionNumber ) {
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
