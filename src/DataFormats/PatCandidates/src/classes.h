#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/Wrapper.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Particle.h"
#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/PatCandidates/interface/Hemisphere.h"

#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"

#include "DataFormats/PatCandidates/interface/StringMap.h"
#include "DataFormats/PatCandidates/interface/EventHypothesis.h"
#include "DataFormats/PatCandidates/interface/EventHypothesisLooper.h"
#include "DataFormats/PatCandidates/interface/TriggerPrimitive.h"

// vvvv Needed to fix dictionaries missing in 169pre2
#include "DataFormats/METReco/interface/MET.h"
#include "DataFormats/METReco/interface/METCollection.h"
#include "DataFormats/METReco/interface/CaloMET.h"
#include "DataFormats/METReco/interface/CaloMETCollection.h"
#include "DataFormats/METReco/interface/GenMET.h"
#include "DataFormats/METReco/interface/GenMETCollection.h"

//#include "DataFormats/BTauReco/interface/JetTagFwd.h"
//#include "DataFormats/BTauReco/interface/JetTag.h"

#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/JetReco/interface/GenJet.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "DataFormats/TauReco/interface/PFTauFwd.h"
#include "DataFormats/TauReco/interface/PFTau.h"
// ^^^^^ End

// we need these typedefs, it won't work directly - NO IDEA WHY!!!
namespace pat {
  typedef edm::Ref<std::vector<pat::Electron> > ElectronRef;
  typedef edm::Ref<std::vector<pat::Muon> >     MuonRef;
  typedef edm::Ref<std::vector<pat::Tau> >      TauRef;
  typedef edm::Ref<std::vector<pat::Photon> >   PhotonRef;
  typedef edm::Ref<std::vector<pat::Jet> >      JetRef;
  typedef edm::Ref<std::vector<pat::MET> >      METRef;
  typedef edm::Ref<std::vector<pat::Particle> > ParticleRef;
  typedef edm::Ref<std::vector<pat::GenericParticle> > GenericParticleRef;
  typedef edm::Ref<std::vector<pat::Hemisphere> > HemisphereRef;

  typedef edm::Ref<std::vector<pat::ElectronType> > ElectronTypeRef;
  typedef edm::Ref<std::vector<pat::MuonType> >     MuonTypeRef;
  typedef edm::Ref<std::vector<pat::TauType> >      TauTypeRef;
  typedef edm::Ref<std::vector<pat::PhotonType> >   PhotonTypeRef;
  typedef edm::Ref<std::vector<pat::JetType> >      JetTypeRef;
  typedef edm::Ref<std::vector<pat::METType> >      METTypeRef;
  typedef edm::Ref<std::vector<pat::ParticleType> > ParticleTypeRef;
}


namespace {
  namespace {

    std::pair<std::string,int32_t> smap0;
    std::vector<std::pair<std::string,int32_t> > smap1;
    StringMap smap;
    edm::Wrapper<StringMap> smap_w;

    std::pair<std::string, edm::Ptr<reco::Candidate> > hypo0;
    std::vector<std::pair<std::string, edm::Ptr<reco::Candidate> > > hypo1;
    pat::EventHypothesis hypot;
    std::vector<pat::EventHypothesis> hypots;
    edm::Wrapper<std::vector<pat::EventHypothesis> > hypots_w;
   
    edm::Wrapper<edm::ValueMap<reco::CandidatePtr> >  candPtrMap_w;

    std::string tp_st;
    pat::TriggerPrimitive tp;
    std::vector<pat::TriggerPrimitive> tpv;
    std::vector<pat::TriggerPrimitive *> tppv;
    pat::TriggerPrimitiveCollection tpc;
    edm::Wrapper<pat::TriggerPrimitiveCollection> tpc_w;
    pat::TriggerPrimitiveMatch tpm;
    edm::Wrapper<pat::TriggerPrimitiveMatch> tpm_w;
    pat::TriggerPrimitiveRef tpr;
    pat::TriggerPrimitiveRefProd tprp;
    pat::TriggerPrimitiveRefVector tprv;
    edm::reftobase::Holder<reco::Candidate, pat::TriggerPrimitiveRef> tpr_h;
    edm::reftobase::RefHolder<pat::TriggerPrimitiveRef> tpr_rh;
    edm::reftobase::VectorHolder<reco::Candidate, pat::TriggerPrimitiveRefVector> tprv_h;
    edm::reftobase::RefVectorHolder<pat::TriggerPrimitiveRefVector> tprv_rh;


    // To check:
    // These don't belong here, and maybe they already exist in the meantime
    std::pair<int,float>  dummy0;
    std::pair<float,float>  dummy0_1;
    std::pair<std::string,float>  dummy0_2;
    std::vector<std::pair<int, float> >    dummy1;
    std::vector<std::vector<std::pair<int, float> >  >  dummy2;
    std::vector<std::pair<float, float> >                 v_p_dbl_dbl;
    std::pair<unsigned int, std::vector<unsigned int> >   p_uint_vint;
    std::vector<std::pair<std::string, float> >           v_p_str_dbl;
    std::vector<std::pair<unsigned int, float> >          v_p_uint_dbl;
    std::pair<unsigned int, float>                        p_uint_dbl;

    edm::Wrapper<std::vector<pat::Electron> >   w_v_el;
    edm::Wrapper<std::vector<pat::Muon> >       w_v_mu;
    edm::Wrapper<std::vector<pat::Tau> >        w_v_tau;
    edm::Wrapper<std::vector<pat::Photon> >     w_v_photon;
    edm::Wrapper<std::vector<pat::Jet> >        w_v_jet;
    edm::Wrapper<std::vector<pat::MET> >        w_v_met;
    edm::Wrapper<std::vector<pat::Particle> >   w_v_part;
    edm::Wrapper<std::vector<pat::GenericParticle> > w_v_gpart;
    edm::Wrapper<std::vector<pat::Hemisphere> > w_v_hemi;
    edm::Ref<std::vector<pat::Electron> >       r_el;
    edm::Ref<std::vector<pat::Muon> >           r_mu;
    edm::Ref<std::vector<pat::Tau> >            r_tau;
    edm::Ref<std::vector<pat::Photon> >         r_photon;
    edm::Ref<std::vector<pat::Jet> >            r_jet;
    edm::Ref<std::vector<pat::MET> >            r_met;
    edm::Ref<std::vector<pat::Particle> >       r_part;
    edm::Ref<std::vector<pat::GenericParticle> > r_gpart;
    edm::Ref<std::vector<pat::Hemisphere> >     r_hemi;

    edm::Wrapper<edm::ValueMap<pat::JetCorrFactors> > wvm_jcf;

    edm::Wrapper<edm::Association<reco::GenJetCollection> > rgjc;
    
    edm::RefToBase<pat::ElectronType>  rbElectron;
    edm::reftobase::IndirectHolder<pat::ElectronType> rbihElectron;
    edm::reftobase::Holder<pat::ElectronType, pat::ElectronTypeRef> rbh1Electron;
    edm::reftobase::Holder<pat::ElectronType, pat::ElectronRef>     rbh2Electron;
    edm::reftobase::RefHolder<pat::ElectronRef> rhElectron;
    edm::reftobase::Holder<reco::Candidate, pat::ElectronRef> rbh3Electron;


    edm::RefToBase<pat::MuonType>  rbMuon;
    edm::reftobase::IndirectHolder<pat::MuonType> rbihMuon;
    edm::reftobase::Holder<pat::MuonType, pat::MuonTypeRef> rbh1Muon;
    edm::reftobase::Holder<pat::MuonType, pat::MuonRef>     rbh2Muon;
    edm::reftobase::RefHolder<pat::MuonRef> rhMuon;
    edm::reftobase::Holder<reco::Candidate, pat::MuonRef> rbh3Muon;

    edm::RefToBase<pat::TauType>  rbTau;
    edm::reftobase::IndirectHolder<pat::TauType> rbihTau;
    edm::reftobase::Holder<pat::TauType, pat::TauTypeRef> rbh1Tau;
    edm::reftobase::Holder<pat::TauType, pat::TauRef>     rbh2Tau;
    edm::reftobase::RefHolder<pat::TauRef> rhTau;
    edm::reftobase::Holder<reco::Candidate, pat::TauRef> rbh3Tau;
    edm::reftobase::RefHolder<reco::PFTauRef> rhPFTau;
    edm::reftobase::Holder<reco::Candidate, reco::PFTauRef> rbh4Tau;

    edm::RefToBase<pat::PhotonType>  rbPhoton;
    edm::reftobase::IndirectHolder<pat::PhotonType> rbihPhoton;
    edm::reftobase::Holder<pat::PhotonType, pat::PhotonTypeRef> rbh1Photon;
    edm::reftobase::Holder<pat::PhotonType, pat::PhotonRef>     rbh2Photon;
    edm::reftobase::RefHolder<pat::PhotonRef> rhPhoton;
    edm::reftobase::Holder<reco::Candidate, pat::PhotonRef> rbh3Photon;

    edm::reftobase::Holder<pat::JetType, pat::JetRef>     rbh2Jet;
    edm::reftobase::RefHolder<pat::JetRef> rhJet;
    edm::reftobase::Holder<reco::Candidate, pat::JetRef> rbh3Jet;

    edm::RefToBase<pat::METType>  rbMET;
    edm::reftobase::IndirectHolder<pat::METType> rbihMET;
    edm::reftobase::Holder<pat::METType, pat::METTypeRef> rbh1MET;
    edm::reftobase::Holder<pat::METType, pat::METRef>     rbh2MET;
    edm::reftobase::RefHolder<pat::METRef> rhMET;
    edm::reftobase::Holder<reco::Candidate, pat::METRef> rbh3MET;

    edm::reftobase::Holder<reco::Candidate, pat::ParticleRef> hParticle;
    edm::reftobase::RefHolder<pat::ParticleRef> rhParticle;

    edm::reftobase::Holder<reco::Candidate, pat::GenericParticleRef> rGParticle;
    edm::reftobase::RefHolder<pat::GenericParticleRef> rhGParticle;

    edm::Wrapper<edm::ValueMap<reco::TrackRefVector> > patJTA;
   
    std::vector<edm::Ref<std::vector<reco::SecondaryVertexTagInfo>,reco::SecondaryVertexTagInfo,edm::refhelper::FindUsingAdvance<std::vector<reco::SecondaryVertexTagInfo>,reco::SecondaryVertexTagInfo> > > rbh1btag ;
    std::vector<edm::Ref<std::vector<reco::SoftLeptonTagInfo>,reco::SoftLeptonTagInfo,edm::refhelper::FindUsingAdvance<std::vector<reco::SoftLeptonTagInfo>,reco::SoftLeptonTagInfo> > > rbh2btag; 

    std::vector<edm::Ref<std::vector<reco::TrackIPTagInfo>,reco::TrackIPTagInfo,edm::refhelper::FindUsingAdvance<std:: vector<reco::TrackIPTagInfo>,reco::TrackIPTagInfo> > > rbh3btag;
    std::vector<std::pair<pat::IsolationKeys,reco::IsoDeposit> > rbh4btag;

    
    std::pair<pat::IsolationKeys,reco::IsoDeposit> rbh4unknown;

    edm::Wrapper<edm::ValueMap<edm::Ptr<reco::BaseTagInfo> > > wbti;

    }
}
