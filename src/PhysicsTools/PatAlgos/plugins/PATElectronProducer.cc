//
// $Id: PATElectronProducer.cc,v 1.37.16.2 2010/04/20 14:46:48 srappocc Exp $
//

#include "PhysicsTools/PatAlgos/plugins/PATElectronProducer.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"

#include "PhysicsTools/PatUtils/interface/TrackerIsolationPt.h"
#include "PhysicsTools/PatUtils/interface/CaloIsolationEnergy.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/VertexReco/interface/Vertex.h"


#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/IPTools/interface/IPTools.h"

#include <vector>
#include <memory>


using namespace pat;
using namespace std;


PATElectronProducer::PATElectronProducer(const edm::ParameterSet & iConfig) :
  isolator_(iConfig.exists("userIsolation") ? iConfig.getParameter<edm::ParameterSet>("userIsolation") : edm::ParameterSet(), false) ,
  useUserData_(iConfig.exists("userData"))
{

  // general configurables
  electronSrc_      = iConfig.getParameter<edm::InputTag>( "electronSource" );
  embedGsfTrack_    = iConfig.getParameter<bool>         ( "embedGsfTrack" );
  embedSuperCluster_= iConfig.getParameter<bool>         ( "embedSuperCluster" );
  embedTrack_       = iConfig.getParameter<bool>         ( "embedTrack" );

  // pflow specific
  pfElecSrc_           = iConfig.getParameter<edm::InputTag>( "pfElectronSource" );
  useParticleFlow_        = iConfig.getParameter<bool>( "useParticleFlow" );
  embedPFCandidate_   = iConfig.getParameter<bool>( "embedPFCandidate" );

  
  // MC matching configurables
  addGenMatch_      = iConfig.getParameter<bool>          ( "addGenMatch" );
  if (addGenMatch_) {
    embedGenMatch_ = iConfig.getParameter<bool>         ( "embedGenMatch" );
    if (iConfig.existsAs<edm::InputTag>("genParticleMatch")) {
      genMatchSrc_.push_back(iConfig.getParameter<edm::InputTag>( "genParticleMatch" ));
    } else {
      genMatchSrc_ = iConfig.getParameter<std::vector<edm::InputTag> >( "genParticleMatch" );
    }
  }

  // resolution configurables
  addResolutions_   = iConfig.getParameter<bool>         ( "addResolutions" );
  if (addResolutions_) {
    resolutionLoader_ = pat::helper::KinResolutionsLoader(iConfig.getParameter<edm::ParameterSet>("resolutions"));
  }


  // electron ID configurables
  addElecID_        = iConfig.getParameter<bool>         ( "addElectronID" );
  if (addElecID_) {
    // it might be a single electron ID
    if (iConfig.existsAs<edm::InputTag>("electronIDSource")) {
      elecIDSrcs_.push_back(NameTag("", iConfig.getParameter<edm::InputTag>("electronIDSource")));
    }
    // or there might be many of them
    if (iConfig.existsAs<edm::ParameterSet>("electronIDSources")) {
      // please don't configure me twice
      if (!elecIDSrcs_.empty()) throw cms::Exception("Configuration") << 
				  "PATElectronProducer: you can't specify both 'electronIDSource' and 'electronIDSources'\n";
      // read the different electron ID names
      edm::ParameterSet idps = iConfig.getParameter<edm::ParameterSet>("electronIDSources");
      std::vector<std::string> names = idps.getParameterNamesForType<edm::InputTag>();
      for (std::vector<std::string>::const_iterator it = names.begin(), ed = names.end(); it != ed; ++it) {
	elecIDSrcs_.push_back(NameTag(*it, idps.getParameter<edm::InputTag>(*it)));
      }
    }
    // but in any case at least once
    if (elecIDSrcs_.empty()) throw cms::Exception("Configuration") <<
			       "PATElectronProducer: id addElectronID is true, you must specify either:\n" <<
			       "\tInputTag electronIDSource = <someTag>\n" << "or\n" <<
			       "\tPSet electronIDSources = { \n" <<
			       "\t\tInputTag <someName> = <someTag>   // as many as you want \n " <<
			       "\t}\n";
  }
  
  // construct resolution calculator

  //   // IsoDeposit configurables
  //   if (iConfig.exists("isoDeposits")) {
  //      edm::ParameterSet depconf = iConfig.getParameter<edm::ParameterSet>("isoDeposits");
  //      if (depconf.exists("tracker")) isoDepositLabels_.push_back(std::make_pair(TrackerIso, depconf.getParameter<edm::InputTag>("tracker")));
  //      if (depconf.exists("ecal"))    isoDepositLabels_.push_back(std::make_pair(ECalIso, depconf.getParameter<edm::InputTag>("ecal")));
  //      if (depconf.exists("hcal"))    isoDepositLabels_.push_back(std::make_pair(HCalIso, depconf.getParameter<edm::InputTag>("hcal")));


  //      if (depconf.exists("user")) {
  //         std::vector<edm::InputTag> userdeps = depconf.getParameter<std::vector<edm::InputTag> >("user");
  //         std::vector<edm::InputTag>::const_iterator it = userdeps.begin(), ed = userdeps.end();
  //         int key = UserBaseIso;
  //         for ( ; it != ed; ++it, ++key) {
  //             isoDepositLabels_.push_back(std::make_pair(IsolationKeys(key), *it));
  //         }
  //      }
  //   }

  // read isoDeposit labels, for direct embedding
  readIsolationLabels(iConfig, "isoDeposits", isoDepositLabels_);

  // read isolation value labels, for direct embedding
  readIsolationLabels(iConfig, "isolationValues", isolationValueLabels_);

  // Efficiency configurables
  addEfficiencies_ = iConfig.getParameter<bool>("addEfficiencies");
  if (addEfficiencies_) {
    efficiencyLoader_ = pat::helper::EfficiencyLoader(iConfig.getParameter<edm::ParameterSet>("efficiencies"));
  }

  // Check to see if the user wants to add user data
  if ( useUserData_ ) {
    userDataHelper_ = PATUserDataHelper<Electron>(iConfig.getParameter<edm::ParameterSet>("userData"));
  }

  // embed high level selection variables?
  embedHighLevelSelection_ = iConfig.getParameter<bool>("embedHighLevelSelection");
  if ( embedHighLevelSelection_ ) {
    beamLineSrc_ = iConfig.getParameter<edm::InputTag>("beamLineSrc");
    usePV_ = iConfig.getParameter<bool>("usePV");
    pvSrc_ = iConfig.getParameter<edm::InputTag>("pvSrc");
  }


  // produces vector of muons
  produces<std::vector<Electron> >();

}


PATElectronProducer::~PATElectronProducer() {
}


void PATElectronProducer::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {

  // Get the collection of electrons from the event
  edm::Handle<edm::View<reco::GsfElectron> > electrons;
  iEvent.getByLabel(electronSrc_, electrons);

  // Get the ESHandle for the transient track builder, if needed for
  // high level selection embedding
  edm::ESHandle<TransientTrackBuilder> trackBuilder;

  if (isolator_.enabled()) isolator_.beginEvent(iEvent,iSetup);

  if (efficiencyLoader_.enabled()) efficiencyLoader_.newEvent(iEvent);
  if (resolutionLoader_.enabled()) resolutionLoader_.newEvent(iEvent, iSetup);

  IsoDepositMaps deposits(isoDepositLabels_.size());
  for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
    iEvent.getByLabel(isoDepositLabels_[j].second, deposits[j]);
  }

  IsolationValueMaps isolationValues(isolationValueLabels_.size());
  for (size_t j = 0; j<isolationValueLabels_.size(); ++j) {
    iEvent.getByLabel(isolationValueLabels_[j].second, isolationValues[j]);
  }  

  // prepare the MC matching
  GenAssociations  genMatches(genMatchSrc_.size());
  if (addGenMatch_) {
    for (size_t j = 0, nd = genMatchSrc_.size(); j < nd; ++j) {
      iEvent.getByLabel(genMatchSrc_[j], genMatches[j]);
    }
  }

  // prepare ID extraction
  std::vector<edm::Handle<edm::ValueMap<float> > > idhandles;
  std::vector<pat::Electron::IdPair>               ids;
  if (addElecID_) {
    idhandles.resize(elecIDSrcs_.size());
    ids.resize(elecIDSrcs_.size());
    for (size_t i = 0; i < elecIDSrcs_.size(); ++i) {
      iEvent.getByLabel(elecIDSrcs_[i].second, idhandles[i]);
      ids[i].first = elecIDSrcs_[i].first;
    }
  }


  // prepare the high level selection:
  // needs beamline
  reco::TrackBase::Point beamPoint(0,0,0);
  reco::Vertex primaryVertex;
  if ( embedHighLevelSelection_ ) {
    // Get the beamspot
    reco::BeamSpot beamSpot;
    edm::Handle<reco::BeamSpot> beamSpotHandle;
    iEvent.getByLabel(beamLineSrc_, beamSpotHandle);


    // Get the primary vertex
    edm::Handle< std::vector<reco::Vertex> > pvHandle;
    iEvent.getByLabel( pvSrc_, pvHandle );
    
    if ( ! usePV_ ) {
      if ( beamSpotHandle.isValid() ){
	beamSpot = *beamSpotHandle;
      } else{
	edm::LogError("DataNotAvailable")
	  << "No beam spot available from EventSetup, not adding high level selection \n";
      }
  
      double x0 = beamSpot.x0();
      double y0 = beamSpot.y0();
      double z0 = beamSpot.z0();
      
      beamPoint = reco::TrackBase::Point ( x0, y0, z0 );
    } else {
      if ( pvHandle.isValid() ) {
	primaryVertex = pvHandle->at(0);
      } else {
	edm::LogError("DataNotAvailable")
	  << "No primary vertex available from EventSetup, not adding high level selection \n";
      }

      // This is needed by the IPTools methods from the tracking group
      iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", trackBuilder);     
    }
  }

  std::vector<Electron> * patElectrons = new std::vector<Electron>();

  if( useParticleFlow_ ) {
    edm::Handle< reco::PFCandidateCollection >  pfElectrons;
    iEvent.getByLabel(pfElecSrc_, pfElectrons);
    unsigned index=0;
  
    for( reco::PFCandidateConstIterator i = pfElectrons->begin(); 
	 i != pfElectrons->end(); ++i, ++index) {

      reco::PFCandidateRef pfRef(pfElectrons, index);
      reco::PFCandidatePtr ptrToPFElectron(pfElectrons,index);
//       reco::CandidateBaseRef pfBaseRef( pfRef ); 
      
      reco::GsfTrackRef PfTk= i->gsfTrackRef(); 

      bool Matched=false;
      for (edm::View<reco::GsfElectron>::const_iterator itElectron = electrons->begin(); itElectron != electrons->end(); ++itElectron) {
	unsigned int idx = itElectron - electrons->begin();
	if (Matched) continue;
	reco::GsfTrackRef EgTk= itElectron->gsfTrack();
	if (itElectron->gsfTrack()==i->gsfTrackRef()){

	  Matched=true;
	  
	  // ptr needed for finding the matched gen particle
	  reco::CandidatePtr ptrToGsfElectron(electrons,idx);

	  // ref to base needed for the construction of the pat object
	  const edm::RefToBase<reco::GsfElectron>& elecsRef = electrons->refAt(idx);
	  Electron anElectron(elecsRef);
	  anElectron.setPFCandidateRef( pfRef  );

	  if( embedPFCandidate_ ) anElectron.embedPFCandidate();

	  //Electron Id

	  if (addElecID_) {
	    //STANDARD EL ID 
	    for (size_t i = 0; i < elecIDSrcs_.size(); ++i) {
	      ids[i].second = (*idhandles[i])[elecsRef];    
	    }
	    //SPECIFIC PF ID
	    ids.push_back(std::make_pair("pf_evspi",pfRef->mva_e_pi()));
	    ids.push_back(std::make_pair("pf_evsmu",pfRef->mva_e_mu()));
	    anElectron.setElectronIDs(ids);
	  }
	  	  
// 	  fillElectron(anElectron,elecsRef,pfBaseRef, 
// 		       genMatches, deposits, isolationValues);

	  //COLIN small warning !
	  // we are currently choosing to take the 4-momentum of the GsfElectron. 
	  // we must therefore match the GsfElectron. 
	  // because of this, we should not change the source of the electron matcher
	  // to the collection of PFElectrons in the python configuration 
	  // I don't know what to do with the efficiencyLoader, since I don't know
	  // what this class is for. 
	  fillElectron2( anElectron, 
			 ptrToPFElectron->sourceCandidatePtr(0), 
			 ptrToGsfElectron, 
			 ptrToGsfElectron, 
			 genMatches, deposits, isolationValues );
	  
	  //COLIN need to use fillElectron2 in the non-pflow case as well, and to test it. 

	  patElectrons->push_back(anElectron);
	}
      }      
    }
  }

  else{
    for (edm::View<reco::GsfElectron>::const_iterator itElectron = electrons->begin(); itElectron != electrons->end(); ++itElectron) {
      // construct the Electron from the ref -> save ref to original object
      unsigned int idx = itElectron - electrons->begin();
      edm::RefToBase<reco::GsfElectron> elecsRef = electrons->refAt(idx);
      reco::CandidateBaseRef elecBaseRef(elecsRef);
      Electron anElectron(elecsRef);    

      // add resolution info
    
      // Isolation
      if (isolator_.enabled()) {
        isolator_.fill(*electrons, idx, isolatorTmpStorage_);
        typedef pat::helper::MultiIsolator::IsolationValuePairs IsolationValuePairs;
        // better to loop backwards, so the vector is resized less times
        for (IsolationValuePairs::const_reverse_iterator it = isolatorTmpStorage_.rbegin(), ed = isolatorTmpStorage_.rend(); it != ed; ++it) {
	  anElectron.setIsolation(it->first, it->second);
        }
      }

      for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
        anElectron.setIsoDeposit(isoDepositLabels_[j].first, (*deposits[j])[elecsRef]);
      }

      // add electron ID info
      if (addElecID_) {
        for (size_t i = 0; i < elecIDSrcs_.size(); ++i) {
	  ids[i].second = (*idhandles[i])[elecsRef];    
        }
        anElectron.setElectronIDs(ids);
      }
    

      if ( useUserData_ ) {
	userDataHelper_.add( anElectron, iEvent, iSetup );
      }
    

      // embed high level selection
      if ( embedHighLevelSelection_ ) {
	// get the global track
	reco::GsfTrackRef track = itElectron->gsfTrack();
      
	// Make sure the collection it points to is there
	if ( track.isNonnull() && track.isAvailable() ) {

	  if ( !usePV_ ) {
	    double corr_d0 = track->dxy( beamPoint );
	    anElectron.setDB( corr_d0, -1.0 );
	  } else {
	    reco::TransientTrack tt = trackBuilder->build(track);
	    std::pair<bool,Measurement1D> result = IPTools::absoluteTransverseImpactParameter(tt, primaryVertex);
	    double d0_corr = result.second.value();
	    double d0_err = result.second.error();
	    anElectron.setDB( d0_corr, d0_err );	    
	  }
	} 
      }
    
      // add sel to selected
      fillElectron( anElectron, elecsRef,elecBaseRef, 
		    genMatches, deposits, isolationValues);
      patElectrons->push_back(anElectron);
    }
  }
  
  // sort electrons in pt
  std::sort(patElectrons->begin(), patElectrons->end(), pTComparator_);

  // add the electrons to the event output
  std::auto_ptr<std::vector<Electron> > ptr(patElectrons);
  iEvent.put(ptr);

  // clean up
  if (isolator_.enabled()) isolator_.endEvent();

}

void PATElectronProducer::fillElectron(Electron& anElectron,
				       const edm::RefToBase<reco::GsfElectron>& elecRef,
				       const reco::CandidateBaseRef& baseRef,
				       const GenAssociations& genMatches, 
				       const IsoDepositMaps& deposits,
				       const IsolationValueMaps& isolationValues
				       ) const {

  //COLIN: might want to use the PFCandidate 4-mom. Which one is in use now?
  //   if (useParticleFlow_) 
  //     aMuon.setP4( aMuon.pfCandidateRef()->p4() );

  //COLIN:
  //In the embedding case, the reference cannot be used to look into a value map. 
  //therefore, one has to had the PFCandidateRef to this function, which becomes a bit
  //too much specific. 

  // in fact, this function needs a baseref or ptr for genmatch
  // and a baseref or ptr for isodeposits and isolationvalues. 
  // baseref is not needed
  // the ptrForIsolation and ptrForMatching should be defined upstream. 

  // is the concrete elecRef needed for the efficiency loader? what is this loader?
  // how can we make it compatible with the particle flow electrons? 

  if (embedGsfTrack_) anElectron.embedGsfTrack();
  if (embedSuperCluster_) anElectron.embedSuperCluster();
  if (embedTrack_) anElectron.embedTrack();

  // store the match to the generated final state muons
  if (addGenMatch_) {
    for(size_t i = 0, n = genMatches.size(); i < n; ++i) {
      if(useParticleFlow_) {
	reco::GenParticleRef genElectron = (*genMatches[i])[anElectron.pfCandidateRef()];
	anElectron.addGenParticleRef(genElectron);	
      }
      else {
	reco::GenParticleRef genElectron = (*genMatches[i])[elecRef];
	anElectron.addGenParticleRef(genElectron);
      }
    }
    if (embedGenMatch_) anElectron.embedGenParticle();
  }

  if (efficiencyLoader_.enabled()) {
    efficiencyLoader_.setEfficiencies( anElectron, elecRef );
  }
    
  if (resolutionLoader_.enabled()) {
    resolutionLoader_.setResolutions(anElectron);
  }

  for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
    if(useParticleFlow_) {

      reco::PFCandidateRef pfcandref =  anElectron.pfCandidateRef();
      assert(!pfcandref.isNull());
      reco::CandidatePtr source = pfcandref->sourceCandidatePtr(0);      
      anElectron.setIsoDeposit(isoDepositLabels_[j].first, 
			  (*deposits[j])[source]);
    }
    else
      anElectron.setIsoDeposit(isoDepositLabels_[j].first,
                          (*deposits[j])[elecRef]);
  }
  
  for (size_t j = 0; j<isolationValues.size(); ++j) {
    if(useParticleFlow_) {
      reco::CandidatePtr source = anElectron.pfCandidateRef()->sourceCandidatePtr(0);      
      anElectron.setIsolation(isolationValueLabels_[j].first, 
			 (*isolationValues[j])[source]);
    }
    else
      anElectron.setIsolation(isolationValueLabels_[j].first,
			 (*isolationValues[j])[elecRef]);
  }



}

void PATElectronProducer::fillElectron2( Electron& anElectron, 
					 const reco::CandidatePtr& candPtrForIsolation,
					 const reco::CandidatePtr& candPtrForGenMatch,
					 const reco::CandidatePtr& candPtrForLoader,
					 const GenAssociations& genMatches, 
					 const IsoDepositMaps& deposits,
					 const IsolationValueMaps& isolationValues) const {
  
  //COLIN: might want to use the PFCandidate 4-mom. Which one is in use now?
  //   if (useParticleFlow_) 
  //     aMuon.setP4( aMuon.pfCandidateRef()->p4() );

  // is the concrete elecRef needed for the efficiency loader? what is this loader?
  // how can we make it compatible with the particle flow electrons? 

  if (embedGsfTrack_) anElectron.embedGsfTrack();
  if (embedSuperCluster_) anElectron.embedSuperCluster();
  if (embedTrack_) anElectron.embedTrack();

  // store the match to the generated final state muons

  if (addGenMatch_) {
    for(size_t i = 0, n = genMatches.size(); i < n; ++i) {
      reco::GenParticleRef genElectron = (*genMatches[i])[candPtrForGenMatch];
      anElectron.addGenParticleRef(genElectron);	
    }
    if (embedGenMatch_) anElectron.embedGenParticle();
  }

  //COLIN what's this? does it have to be GsfElectron specific?
  if (efficiencyLoader_.enabled()) {
    efficiencyLoader_.setEfficiencies( anElectron, candPtrForLoader );
  }
    
  if (resolutionLoader_.enabled()) {
    resolutionLoader_.setResolutions(anElectron);
  }

  for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
    if( isoDepositLabels_[j].first==pat::TrackIso ||
	isoDepositLabels_[j].first==pat::EcalIso ||
	isoDepositLabels_[j].first==pat::HcalIso ) {

      anElectron.setIsoDeposit(isoDepositLabels_[j].first, 
 			       (*deposits[j])[candPtrForGenMatch]);
    }
    else {
      anElectron.setIsoDeposit(isoDepositLabels_[j].first, 
 			       (*deposits[j])[candPtrForIsolation]);
    }
  }
  
  for (size_t j = 0; j<isolationValues.size(); ++j) {
    if( isolationValueLabels_[j].first==pat::TrackIso ||
	isolationValueLabels_[j].first==pat::EcalIso ||
	isolationValueLabels_[j].first==pat::HcalIso ) {
      anElectron.setIsolation(isolationValueLabels_[j].first,
 			      (*isolationValues[j])[candPtrForGenMatch]);
    }
    else {
      anElectron.setIsolation(isolationValueLabels_[j].first,
 			      (*isolationValues[j])[candPtrForIsolation]);
    }
  }
}


// ParameterSet description for module
void PATElectronProducer::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription iDesc;
  iDesc.setComment("PAT electron producer module");

  // input source 
  iDesc.add<edm::InputTag>("electronSource", edm::InputTag("no default"))->setComment("input collection");

  // embedding
  iDesc.add<bool>("embedGsfTrack", true)->setComment("embed external gsf track");
  iDesc.add<bool>("embedSuperCluster", true)->setComment("embed external super cluster");
  iDesc.add<bool>("embedTrack", false)->setComment("embed external track");

  // pf specific parameters
  iDesc.add<edm::InputTag>("pfElectronSource", edm::InputTag("pfElectrons"))->setComment("particle flow input collection");
  iDesc.add<bool>("useParticleFlow", false)->setComment("whether to use particle flow or not");
  iDesc.add<bool>("embedPFCandidate", false)->setComment("embed external particle flow object");

  // MC matching configurables
  iDesc.add<bool>("addGenMatch", true)->setComment("add MC matching");
  iDesc.add<bool>("embedGenMatch", false)->setComment("embed MC matched MC information");
  std::vector<edm::InputTag> emptySourceVector;
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("genParticleMatch", edm::InputTag(), true) xor 
                 edm::ParameterDescription<std::vector<edm::InputTag> >("genParticleMatch", emptySourceVector, true)
		 )->setComment("input with MC match information");

  // electron ID configurables
  iDesc.add<bool>("addElectronID",true)->setComment("add electron ID variables");
  edm::ParameterSetDescription electronIDSourcesPSet;
  electronIDSourcesPSet.setAllowAnything(); 
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("electronIDSource", edm::InputTag(), true) xor
                 edm::ParameterDescription<edm::ParameterSetDescription>("electronIDSources", electronIDSourcesPSet, true)
                 )->setComment("input with electron ID variables");


  // IsoDeposit configurables
  edm::ParameterSetDescription isoDepositsPSet;
  isoDepositsPSet.addOptional<edm::InputTag>("tracker"); 
  isoDepositsPSet.addOptional<edm::InputTag>("ecal");
  isoDepositsPSet.addOptional<edm::InputTag>("hcal");
  isoDepositsPSet.addOptional<edm::InputTag>("pfAllParticles");
  isoDepositsPSet.addOptional<edm::InputTag>("pfChargedHadrons");
  isoDepositsPSet.addOptional<edm::InputTag>("pfNeutralHadrons");
  isoDepositsPSet.addOptional<edm::InputTag>("pfPhotons");
  isoDepositsPSet.addOptional<std::vector<edm::InputTag> >("user");
  iDesc.addOptional("isoDeposits", isoDepositsPSet);

  // isolation values configurables
  edm::ParameterSetDescription isolationValuesPSet;
  isolationValuesPSet.addOptional<edm::InputTag>("tracker"); 
  isolationValuesPSet.addOptional<edm::InputTag>("ecal");
  isolationValuesPSet.addOptional<edm::InputTag>("hcal");
  isolationValuesPSet.addOptional<edm::InputTag>("pfAllParticles");
  isolationValuesPSet.addOptional<edm::InputTag>("pfChargedHadrons");
  isolationValuesPSet.addOptional<edm::InputTag>("pfNeutralHadrons");
  isolationValuesPSet.addOptional<edm::InputTag>("pfPhotons");
  iDesc.addOptional("isolationValues", isolationValuesPSet);

  // Efficiency configurables
  edm::ParameterSetDescription efficienciesPSet;
  efficienciesPSet.setAllowAnything(); // TODO: the pat helper needs to implement a description.
  iDesc.add("efficiencies", efficienciesPSet);
  iDesc.add<bool>("addEfficiencies", false);

  // Check to see if the user wants to add user data
  edm::ParameterSetDescription userDataPSet;
  PATUserDataHelper<Electron>::fillDescription(userDataPSet);
  iDesc.addOptional("userData", userDataPSet);

  // electron shapes
  iDesc.add<bool>("addElectronShapes", true);
  iDesc.add<edm::InputTag>("reducedBarrelRecHitCollection", edm::InputTag("reducedEcalRecHitsEB"));
  iDesc.add<edm::InputTag>("reducedEndcapRecHitCollection", edm::InputTag("reducedEcalRecHitsEE"));

  edm::ParameterSetDescription isolationPSet;
  isolationPSet.setAllowAnything(); // TODO: the pat helper needs to implement a description.
  iDesc.add("userIsolation", isolationPSet);

  // Resolution configurables
  pat::helper::KinResolutionsLoader::fillDescription(iDesc);

  iDesc.add<bool>("embedHighLevelSelection", true)->setComment("embed high level selection");
  edm::ParameterSetDescription highLevelPSet;
  highLevelPSet.setAllowAnything();
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("beamLineSrc", edm::InputTag(), true) 
                 )->setComment("input with high level selection");
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("pvSrc", edm::InputTag(), true) 
                 )->setComment("input with high level selection");
  iDesc.addNode( edm::ParameterDescription<bool>("usePV", bool(), true) 
                 )->setComment("input with high level selection, use primary vertex (true) or beam line (false)");

  descriptions.add("PATElectronProducer", iDesc);

}



void PATElectronProducer::readIsolationLabels( const edm::ParameterSet & iConfig,
					       const char* psetName, 
					       IsolationLabels& labels) {
  
  labels.clear();
  
  if (iConfig.exists( psetName )) {
    edm::ParameterSet depconf 
      = iConfig.getParameter<edm::ParameterSet>(psetName);

    if (depconf.exists("tracker")) labels.push_back(std::make_pair(pat::TrackIso, depconf.getParameter<edm::InputTag>("tracker")));
    if (depconf.exists("ecal"))    labels.push_back(std::make_pair(pat::EcalIso, depconf.getParameter<edm::InputTag>("ecal")));
    if (depconf.exists("hcal"))    labels.push_back(std::make_pair(pat::HcalIso, depconf.getParameter<edm::InputTag>("hcal")));
    if (depconf.exists("pfAllParticles"))  {
      labels.push_back(std::make_pair(pat::PfAllParticleIso, depconf.getParameter<edm::InputTag>("pfAllParticles")));
    }
    if (depconf.exists("pfChargedHadrons"))  {
      labels.push_back(std::make_pair(pat::PfChargedHadronIso, depconf.getParameter<edm::InputTag>("pfChargedHadrons")));
    }
    if (depconf.exists("pfNeutralHadrons"))  {
      labels.push_back(std::make_pair(pat::PfNeutralHadronIso, depconf.getParameter<edm::InputTag>("pfNeutralHadrons")));
    }
    if (depconf.exists("pfPhotons")) {
      labels.push_back(std::make_pair(pat::PfGammaIso, depconf.getParameter<edm::InputTag>("pfPhotons")));
    }
    if (depconf.exists("user")) {
      std::vector<edm::InputTag> userdeps = depconf.getParameter<std::vector<edm::InputTag> >("user");
      std::vector<edm::InputTag>::const_iterator it = userdeps.begin(), ed = userdeps.end();
      int key = UserBaseIso;
      for ( ; it != ed; ++it, ++key) {
	labels.push_back(std::make_pair(IsolationKeys(key), *it));
      }
    }
  }  
  

}


#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PATElectronProducer);
