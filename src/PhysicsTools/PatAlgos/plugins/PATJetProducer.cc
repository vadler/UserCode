//
// $Id$
//

#include "PhysicsTools/PatAlgos/plugins/PATJetProducer.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Candidate/interface/CandAssociation.h"

#include "DataFormats/BTauReco/interface/JetTagFwd.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "DataFormats/BTauReco/interface/TrackProbabilityTagInfo.h"
#include "DataFormats/BTauReco/interface/TrackProbabilityTagInfoFwd.h"
#include "DataFormats/BTauReco/interface/TrackCountingTagInfo.h"
#include "DataFormats/BTauReco/interface/TrackCountingTagInfoFwd.h"
#include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"
#include "DataFormats/BTauReco/interface/SoftLeptonTagInfoFwd.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "PhysicsTools/Utilities/interface/DeltaR.h"

#include "PhysicsTools/PatUtils/interface/RefHelper.h"
#include "PhysicsTools/PatUtils/interface/ObjectResolutionCalc.h"
#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"

#include <vector>
#include <memory>


using namespace pat;


PATJetProducer::PATJetProducer(const edm::ParameterSet& iConfig) {
  // initialize the configurables
  jetsSrc_                 = iConfig.getParameter<edm::InputTag>            ( "jetSource" );
  embedCaloTowers_         = iConfig.getParameter<bool>                     ( "embedCaloTowers" );
  getJetMCFlavour_         = iConfig.getParameter<bool>                     ( "getJetMCFlavour" );
  jetPartonMapSource_      = iConfig.getParameter<edm::InputTag>            ( "JetPartonMapSource" );
  addGenPartonMatch_       = iConfig.getParameter<bool>                     ( "addGenPartonMatch" );
  genPartonSrc_            = iConfig.getParameter<edm::InputTag>            ( "genPartonMatch" );
  addGenJetMatch_          = iConfig.getParameter<bool>                     ( "addGenJetMatch" );
  genJetSrc_               = iConfig.getParameter<edm::InputTag>            ( "genJetMatch" );
  addPartonJetMatch_       = iConfig.getParameter<bool>                     ( "addPartonJetMatch" );
  partonJetSrc_            = iConfig.getParameter<edm::InputTag>            ( "partonJetSource" );
  // Trigger matching configurables
  addTrigMatch_            = iConfig.getParameter<bool>                     ( "addTrigMatch" );
  trigPrimSrc_             = iConfig.getParameter<std::vector<edm::InputTag> >( "trigPrimMatch" );
  jetCorrFactorsSrc_       = iConfig.getParameter<edm::InputTag>            ( "jetCorrFactorsSource" );
  addResolutions_          = iConfig.getParameter<bool>                     ( "addResolutions" );
  useNNReso_               = iConfig.getParameter<bool>                     ( "useNNResolutions" );
  caliJetResoFile_         = iConfig.getParameter<std::string>              ( "caliJetResoFile" );
  caliBJetResoFile_        = iConfig.getParameter<std::string>              ( "caliBJetResoFile" );
  addBTagInfo_             = iConfig.getParameter<bool>                     ( "addBTagInfo" );
  tagModuleLabelPostfix_   = iConfig.getParameter<std::string>              ( "tagModuleLabelPostfix" ); 
  addDiscriminators_       = iConfig.getParameter<bool>                     ( "addDiscriminators" );
  addJetTagRefs_           = iConfig.getParameter<bool>                     ( "addJetTagRefs" );
  tagModuleLabelsToKeep_   = iConfig.getParameter<std::vector<std::string> >( "tagModuleLabelsToKeep" );
  addAssociatedTracks_     = iConfig.getParameter<bool>                     ( "addAssociatedTracks" ); 
  trackAssociationPSet_    = iConfig.getParameter<edm::ParameterSet>        ( "trackAssociation" );
  addJetCharge_            = iConfig.getParameter<bool>                     ( "addJetCharge" ); 
  jetChargePSet_           = iConfig.getParameter<edm::ParameterSet>        ( "jetCharge" );

  // construct resolution calculator
  if (addResolutions_) {
    theResoCalc_ = new ObjectResolutionCalc(edm::FileInPath(caliJetResoFile_).fullPath(), useNNReso_);
    theBResoCalc_ = new ObjectResolutionCalc(edm::FileInPath(caliBJetResoFile_).fullPath(), useNNReso_);
  }

  // construct Jet Track Associator
  simpleJetTrackAssociator_ = ::helper::SimpleJetTrackAssociator(trackAssociationPSet_);
  // construct Jet Charge Computer
  if (addJetCharge_) jetCharge_ = new JetCharge(jetChargePSet_);
 
  // produces vector of jets
  produces<std::vector<Jet> >();
}


PATJetProducer::~PATJetProducer() {
  if (addResolutions_) {
    delete theResoCalc_;
    delete theBResoCalc_;
  }
  if (addJetCharge_) delete jetCharge_;
}


void PATJetProducer::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {

  // Get the vector of jets
  edm::Handle<edm::View<JetType> > jets;
  iEvent.getByLabel(jetsSrc_, jets);
  // get the back-references produced by cleaners, to access associations
  edm::Handle<reco::CandRefValueMap> backRefJets;
  iEvent.getByLabel(jetsSrc_, backRefJets);
  pat::helper::RefHelper<reco::Candidate> backRefHelper(*backRefJets);

  // for jet flavour
  edm::Handle<reco::CandMatchMap> JetPartonMap;
  if (getJetMCFlavour_) iEvent.getByLabel (jetPartonMapSource_, JetPartonMap);

  // Get the vector of generated particles from the event if needed
  edm::Handle<edm::Association<reco::GenParticleCollection> > partonMatch;
  if (addGenPartonMatch_) iEvent.getByLabel(genPartonSrc_,  partonMatch);
  // Get the vector of GenJets from the event if needed
  edm::Handle<edm::Association<reco::GenJetCollection> > genJetMatch;
  if (addGenJetMatch_) iEvent.getByLabel(genJetSrc_, genJetMatch);
/* TO BE IMPLEMENTED FOR >= 1_5_X
  // Get the vector of PartonJets from the event if needed
  edm::Handle<edm::View<reco::SomePartonJetType> > partonJets;
  if (addPartonJetMatch_) iEvent.getByLabel(partonJetSrc_, partonJets);
*/

  // read in the jet correction factors ValueMap
  edm::Handle<edm::ValueMap<JetCorrFactors> > jetCorrs;
  iEvent.getByLabel(jetCorrFactorsSrc_, jetCorrs);

  // Get the vector of jet tags with b-tagging info
  //std::vector<edm::Handle<std::vector<reco::JetTag> > > jetTags_testManyByType ;
  std::vector<edm::Handle<edm::ValueMap<reco::JetTagRef> > > jetTags_testManyByType ;
  iEvent.getManyByType(jetTags_testManyByType);
  // Define the handles for the specific algorithms
  edm::Handle<reco::SoftLeptonTagInfoCollection> jetsInfoHandle_sl;
  edm::Handle<reco::TrackProbabilityTagInfoCollection> jetsInfoHandleTP;
  edm::Handle<reco::TrackCountingTagInfoCollection> jetsInfoHandleTC;

  // tracks Jet Track Association, by hand in CMSSW_1_3_X
  edm::Handle<reco::TrackCollection> hTracks;
  iEvent.getByLabel(trackAssociationPSet_.getParameter<edm::InputTag>("tracksSource"), hTracks);


  // loop over jets
  std::vector<Jet> * patJets = new std::vector<Jet>(); 
  for (edm::View<JetType>::const_iterator itJet = jets->begin(); itJet != jets->end(); itJet++) {

    // construct the Jet from the ref -> save ref to original object
    unsigned int idx = itJet - jets->begin();
    edm::RefToBase<JetType> jetRef = jets->refAt(idx); 
    Jet ajet(jetRef);
    // ensure the internal storage of the jet constituents
    if (embedCaloTowers_) ajet.setCaloTowers(jetRef->getConstituents());

    // calculate the energy correction factors
    JetCorrFactors jcf = backRefHelper.recursiveLookup(jetRef, *jetCorrs);
    ajet.setP4(jcf.scaleDefault() * itJet->p4());
    ajet.setNoCorrFactor(1./jcf.scaleDefault());
    ajet.setJetCorrFactors(jcf);

    // get the MC flavour information for this jet
    if (getJetMCFlavour_) {
      for (reco::CandMatchMap::const_iterator f = JetPartonMap->begin(); f != JetPartonMap->end(); f++) {
        const reco::Candidate * jetClone = f->key->masterClone().get();
        // if (jetClone == &(*itJet) { // comparison by address doesn't work
        // ugly matching!!! bah bah bah!!! but what else...?
        if (fabs(jetClone->eta() - itJet->eta()) < 0.001 &&
            fabs(jetClone->phi() - itJet->phi()) < 0.001) {
          ajet.setPartonFlavour(f->val->pdgId());
        }
      }
    }
    // do the parton matching
    if (addGenPartonMatch_) {
      reco::GenParticleRef parton = (*partonMatch)[jetRef];
      if (parton.isNonnull() && parton.isAvailable()) {
          ajet.setGenParton(*parton);
      } else {
          reco::Particle bestParton(0, reco::Particle::LorentzVector(0, 0, 0, 0), reco::Particle::Point(0,0,0), 0, 0, true);
          ajet.setGenParton(bestParton);
      }
    }
    // do the GenJet matching
    if (addGenJetMatch_) {
      reco::GenJetRef genjet = (*genJetMatch)[jetRef];
      if (genjet.isNonnull() && genjet.isAvailable()) {
          ajet.setGenJet(*genjet);
      } else {
          ajet.setGenJet(reco::GenJet());
      }
    }

    // TO BE IMPLEMENTED FOR >=1_5_X: do the PartonJet matching
    if (addPartonJetMatch_) {
    }
    // matches to fired trigger primitives
    if ( addTrigMatch_ ) {
      for ( size_t i = 0; i < trigPrimSrc_.size(); ++i ) {
        edm::Handle<edm::Association<TriggerPrimitiveCollection> > trigMatch;
        iEvent.getByLabel(trigPrimSrc_[i], trigMatch);
        TriggerPrimitiveRef trigPrim = (*trigMatch)[jetRef];
        if ( trigPrim.isNonnull() && trigPrim.isAvailable() ) {
          ajet.addTriggerMatch(*trigPrim);
        }
      }
    }

    // add resolution info if demanded
    if (addResolutions_) {
      (*theResoCalc_)(ajet);
      Jet abjet(ajet.bCorrJet());
      (*theBResoCalc_)(abjet);
      ajet.setBResolutions(abjet.resolutionEt(), abjet.resolutionEta(), abjet.resolutionPhi(), abjet.resolutionA(), abjet.resolutionB(), abjet.resolutionC(), abjet.resolutionD(), abjet.resolutionTheta());
    }

    // add b-tag info if available & required
    if (addBTagInfo_) {
      for (size_t k=0; k<jetTags_testManyByType.size(); k++) {
        edm::Handle<edm::ValueMap<reco::JetTagRef> > jetTags = jetTags_testManyByType[k];

        //get label and module names
        std::string moduleLabel = (jetTags).provenance()->moduleLabel();

        //look only at the tagger present in tagModuleLabelsToKeep_
        for (unsigned int i = 0; i < tagModuleLabelsToKeep_.size(); ++i) {
          if (moduleLabel == tagModuleLabelsToKeep_[i] + tagModuleLabelPostfix_) {  
              //********store discriminators*********
              if (addDiscriminators_) {
                  std::pair<std::string, float> pairDiscri;
                  pairDiscri.first = tagModuleLabelsToKeep_[i];
                  pairDiscri.second = ((*jetTags)[jetRef])->discriminator();
                  ajet.addBDiscriminatorPair(pairDiscri);
                  continue;
              }
              //********store jetTagRef*********
              if (addJetTagRefs_) {
                  std::pair<std::string, reco::JetTagRef> pairjettagref;
                  pairjettagref.first = tagModuleLabelsToKeep_[i];
                  pairjettagref.second = ((*jetTags)[jetRef]);
                  ajet.addBJetTagRefPair(pairjettagref);
              }
          }
        }
      }
    }

    // Associate tracks with jet (at least temporary)
    simpleJetTrackAssociator_.associate(ajet.momentum(), hTracks, ajet.associatedTracks_);

    // PUT HERE EVERYTHING WHICH NEEDS TRACKS
    if (addJetCharge_) {
      ajet.setJetCharge(static_cast<float>(jetCharge_->charge(ajet.p4(), ajet.associatedTracks())));
    }

    // drop jet track association if the user does not want it
    if (!addAssociatedTracks_) ajet.associatedTracks_.clear();

    patJets->push_back(ajet);
  }

  // sort jets in Et
  std::sort(patJets->begin(), patJets->end(), eTComparator_);

  // put genEvt  in Event
  std::auto_ptr<std::vector<Jet> > myJets(patJets);
  iEvent.put(myJets);

}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PATJetProducer);
