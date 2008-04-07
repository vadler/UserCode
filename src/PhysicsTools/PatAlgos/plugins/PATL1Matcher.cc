//
// $Id$
//


#include "PhysicsTools/PatAlgos/plugins/PATCandMatcher.h"
#include "PhysicsTools/PatAlgos/plugins/PATL1MatchSelector.h"
#include "PhysicsTools/PatUtils/interface/PATMatchByDRDPt.h"
#include "PhysicsTools/PatUtils/interface/PATMatchLessByDPt.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"

// Match by deltaR and deltaPt, ranking by deltaR (default)
typedef pat::PATCandMatcher<
  reco::CandidateView,
  pat::TriggerPrimitiveCollection,
  pat::PATL1MatchSelector< reco::CandidateView::value_type,
                           pat::TriggerPrimitiveCollection::value_type >,
  pat::PATMatchByDRDPt< reco::CandidateView::value_type,
                        pat::TriggerPrimitiveCollection::value_type >
> PATL1Matcher;

// Alternative: match by deltaR and deltaPt, ranking by deltaPt
typedef pat::PATCandMatcher<
  reco::CandidateView,
  pat::TriggerPrimitiveCollection,
  pat::PATL1MatchSelector< reco::CandidateView::value_type,
                           pat::TriggerPrimitiveCollection::value_type>,
  pat::PATMatchByDRDPt< reco::CandidateView::value_type,
                        pat::TriggerPrimitiveCollection::value_type >,
  pat::PATMatchLessByDPt< reco::CandidateView,
                          pat::TriggerPrimitiveCollection >
> PATL1MatcherByPt;


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE( PATL1Matcher );
DEFINE_FWK_MODULE( PATL1MatcherByPt );

