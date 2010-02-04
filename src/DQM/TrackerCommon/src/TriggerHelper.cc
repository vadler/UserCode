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


/// Was this event accepted by the configured L1 logical expression combination?
bool TriggerHelper::acceptL1( const Event & event, const EventSetup & setup, const ParameterSet & config )
{

  // Getting configuration parameters
  const vector< string > l1LogicalExpressions( config.getParameter< vector< string > >( "l1Algorithms" ) );
  errorReplyL1_ = config.getParameter< bool >( "errorReplyL1" );

  // An empty L1 logical expressions list acts as switch.
  if ( l1LogicalExpressions.empty() ) return true;

  // Getting the L1 event setup
  l1Gt_.retrieveL1EventSetup( setup );

  // Determine decision of L1 logical expression combination and return
  if ( config.getParameter< bool >( "andOrL1" ) ) { // OR combination
    for ( vector< string >::const_iterator l1LogicalExpression = l1LogicalExpressions.begin(); l1LogicalExpression != l1LogicalExpressions.end(); ++l1LogicalExpression ) {
      if ( acceptL1LogicalExpression( event, *l1LogicalExpression ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator l1LogicalExpression = l1LogicalExpressions.begin(); l1LogicalExpression != l1LogicalExpressions.end(); ++l1LogicalExpression ) {
    if ( ! acceptL1LogicalExpression( event, *l1LogicalExpression ) ) return false;
  }
  return true;

}


/// Was this event accepted by this particular L1 algorithms' logical expression?
bool TriggerHelper::acceptL1LogicalExpression( const Event & event, string l1LogicalExpression )
{


  // Check empty strings
  if ( l1LogicalExpression.empty() ) {
    LogError( "l1LogicalExpression" ) << "Empty logical expression ==> decision: " << errorReplyL1_;
    return errorReplyL1_;
  }

  // Negated logical expression
  bool negExpr( negate( l1LogicalExpression ) );
  if ( negExpr && l1LogicalExpression.empty() ) {
    LogError( "l1LogicalExpression" ) << "Empty (negated) logical expression ==> decision: " << errorReplyL1_;
    return errorReplyL1_;
  }

  // Parse logical expression and determine L1 decision
  L1GtLogicParser l1AlgoLogicParser( l1LogicalExpression );
  // Loop over algorithms
  for ( size_t iAlgorithm = 0; iAlgorithm < l1AlgoLogicParser.operandTokenVector().size(); ++iAlgorithm ) {
    const string l1AlgoName( l1AlgoLogicParser.operandTokenVector().at( iAlgorithm ).tokenName );
    int error( -1 );
    const bool decision( l1Gt_.decision( event, l1AlgoName, error ) );
    // Error checks
    if ( error != 0 ) {
      if ( error == 1 ) LogError( "l1AlgorithmInMenu" ) << "L1 algorithm " << l1AlgoName << " does not exist in the L1 menu ==> decision: "                                          << errorReplyL1_;
      else              LogError( "l1AlgorithmError" )  << "L1 algorithm " << l1AlgoName << " received error code " << error << " from L1GtUtils::decisionBeforeMask ==> decision: " << errorReplyL1_;
      l1AlgoLogicParser.operandTokenVector().at( iAlgorithm ).tokenResult = errorReplyL1_;
      continue;
    }
    // Manipulate algo decision as stored in the parser
    cout << "  TriggerHelper: algo name " << l1AlgoName << " -> " << decision << endl; // DEBUG
    l1AlgoLogicParser.operandTokenVector().at( iAlgorithm ).tokenResult = decision;
  }

  // Return decision
  const bool l1Decision( l1AlgoLogicParser.expressionResult() );
  cout << "  TriggerHelper: L1 logical expression "; // DEBUG
  if ( negExpr ) cout << "~\"" << l1LogicalExpression << "\" -> " << ( ! l1Decision ) << endl; // DEBUG
  else           cout <<  "\"" << l1LogicalExpression << "\" -> " <<     l1Decision   << endl; // DEBUG
  return negExpr ? ( ! l1Decision ) : l1Decision;

}


/// Was this event accepted by the configured HLT logical expression combination?
bool TriggerHelper::acceptHlt( const Event & event, const ParameterSet & config )
{

  // Getting configuration parameters
  const vector< string > hltLogicalExpressions( config.getParameter< vector< string > >( "hltPaths" ) );
  hltInputTag_   = config.getParameter< InputTag >( "hltInputTag" );
  errorReplyHlt_ = config.getParameter< bool >( "errorReplyHlt" );

  // An empty HLT logical expressions list acts as switch.
  if ( hltLogicalExpressions.empty() ) return true;

  // Checking the TriggerResults InputTag
  // The process name has to be given.
  if ( hltInputTag_.process().size() == 0 ) {
    LogError( "hltProcess" ) << "HLT TriggerResults InputTag " << hltInputTag_.encode() << " specifies no process ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }

  // Accessing the TriggerResults
  event.getByLabel( hltInputTag_, hltTriggerResults_ );
  if ( ! hltTriggerResults_.isValid() ) {
    LogError( "triggerResultsValid" ) << "TriggerResults product with InputTag " << hltInputTag_.encode() << " not in event ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }

  // Getting the HLT configuration from the provenance
  bool changed( true );
  if ( ! hltConfig_.init( event, hltInputTag_.process(), changed ) ) {
    LogError( "hltConfigInit" ) << "HLT config initialization error with process name " << hltInputTag_.process() << " ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }
  if ( hltConfig_.size() <= 0 ) {
    LogError( "hltConfigSize" ) << "HLT config size error ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }

  // Determine decision of HLT logical expression combination and return
  if ( config.getParameter< bool >( "andOrHlt" ) ) { // OR combination
    for ( vector< string >::const_iterator hltLogicalExpression = hltLogicalExpressions.begin(); hltLogicalExpression != hltLogicalExpressions.end(); ++hltLogicalExpression ) {
      if ( acceptHltLogicalExpression( *hltLogicalExpression ) ) return true;
    }
    return false;
  }
  for ( vector< string >::const_iterator hltLogicalExpression = hltLogicalExpressions.begin(); hltLogicalExpression != hltLogicalExpressions.end(); ++hltLogicalExpression ) {
    if ( ! acceptHltLogicalExpression( *hltLogicalExpression ) ) return false;
  }
  return true;

}


/// Was this event accepted by this particular HLT paths' logical expression?
bool TriggerHelper::acceptHltLogicalExpression( string hltLogicalExpression ) const
{

  // Check empty strings
  if ( hltLogicalExpression.empty() ) {
    LogError( "hltLogicalExpression" ) << "Empty logical expression ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }

  // Negated paths
  bool negExpr( negate( hltLogicalExpression ) );
  if ( negExpr && hltLogicalExpression.empty() ) {
    LogError( "hltLogicalExpression" ) << "Empty (negated) logical expression ==> decision: " << errorReplyHlt_;
    return errorReplyHlt_;
  }

  // Parse logical expression and determine HLT decision
  L1GtLogicParser hltAlgoLogicParser( hltLogicalExpression );
  // Loop over paths
  for ( size_t iPath = 0; iPath < hltAlgoLogicParser.operandTokenVector().size(); ++iPath ) {
    const string hltPathName( hltAlgoLogicParser.operandTokenVector().at( iPath ).tokenName );
    const unsigned indexPath( hltConfig_.triggerIndex( hltPathName ) );
    // Further error checks
    if ( indexPath == hltConfig_.size() ) {
      LogError( "hltPathInProcess" ) << "HLT path " << hltPathName << " is not found in process " << hltInputTag_.process() << " ==> decision: " << errorReplyHlt_;
      hltAlgoLogicParser.operandTokenVector().at( iPath ).tokenResult = errorReplyHlt_;
      continue;
    }
    if ( hltTriggerResults_->error( indexPath ) ) {
      LogError( "hltPathError" ) << "HLT path " << hltPathName << " in error ==> decision: " << errorReplyHlt_;
      hltAlgoLogicParser.operandTokenVector().at( iPath ).tokenResult = errorReplyHlt_;
      continue;
    }
    // Manipulate algo decision as stored in the parser
    const bool decision( hltTriggerResults_->accept( indexPath ) );
    cout << "  TriggerHelper: path name " << hltPathName << " -> " << decision << endl; // DEBUG
    hltAlgoLogicParser.operandTokenVector().at( iPath ).tokenResult = decision;
  }

  // Determine decision
  const bool hltDecision( hltAlgoLogicParser.expressionResult() );
  cout << "  TriggerHelper: HLT logical expression "; // DEBUG
  if ( negExpr ) cout << "~\"" << hltLogicalExpression << "\" -> " << ( ! hltDecision ) << endl; // DEBUG
  else           cout <<  "\"" << hltLogicalExpression << "\" -> " <<     hltDecision   << endl; // DEBUG
  return negExpr ? ( ! hltDecision ) : hltDecision;

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
    LogError( "dcsStatusValid" ) << "DcsStatusCollection product with InputTag " << dcsInputTag.encode() << " not in event ==> decision: " << errorReplyDcs_;
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
      LogError( "dcsPartition" ) << "DCS partition number " << dcsPartition << " does not exist ==> decision: " << errorReplyDcs_;
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
