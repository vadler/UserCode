#ifndef TtSpinCorrelations_AnalyzeGenEvent_h
#define TtSpinCorrelations_AnalyzeGenEvent_h


//
// Original Author:  Volker Adler
//         Created:  Fri Feb  8 14:48:21 CET 2008
// $Id$
//


#include <memory>
#include <string>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DataFormats/Candidate/interface/Particle.h"
#include "DataFormats/Math/interface/Vector3D.h"
#include <Math/VectorUtil.h>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TF2.h"

#include "AnalysisDataFormats/TopObjects/interface/TtGenEvent.h"


class AnalyzeGenEvent : public edm::EDAnalyzer {

  public:

    // constructors
    explicit AnalyzeGenEvent(const edm::ParameterSet& iConfig);

    // destructor
    ~AnalyzeGenEvent();

  private:

    // methods
    void beginJob(const edm::EventSetup& iSetup);
    virtual void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup);
    void endJob();

    // generic configurables
    bool useHelBasis_,
         useBeamBasis_,
         useOffDiagBasis_,
         useOptBasis_,
         splitTtAbsolute_;
    edm::InputTag srcGenEvt_;
    std::string filePath_;

    // configurables related to analysis & histos
    double splitTtValue_;            // tt mass to split analysis/reference sample
    int    binsTtMass_;              // binning of tt mass histo
    double xMinTtMass_, xMaxTtMass_; // range of tt mass histo
    int    binsCosAngle1D_;          // binning of 1-dim pseudo-angle histos
    int    binsCosAngle2D_;          // binning of 2-dim pseudo-angle histos

    // output entities
    TFile* file_;
    TTree* tree_;
    TTree* treeMeta_;
    TTree* treeHel_;
    TTree* treeBeam_;
    TTree* treeOffDiag_;
    TTree* treeOpt_;

    // common histos
    TH1F* hTtMass_; // tt mass
    TH1F* hCosTLHel_;              // pseudo-angle l/spin in hel. basis
    TH1F* hCosTBHel_;              // pseudo-angle b/spin in hel. basis
    TH1F* hCosTQHel_;              // pseudo-angle q/spin in hel. basis
    TH1F* hAnaCosTLHel_;           // pseudo-angle l/spin in hel. basis ana. sample
    TH1F* hAnaCosTBHel_;           // pseudo-angle b/spin in hel. basis ana. sample
    TH1F* hAnaCosTQHel_;           // pseudo-angle q/spin in hel. basis ana. sample
    TH1F* hRefCosTLHel_;           // pseudo-angle l/spin in hel. basis ref. sample
    TH1F* hRefCosTBHel_;           // pseudo-angle b/spin in hel. basis ref. sample
    TH1F* hRefCosTQHel_;           // pseudo-angle q/spin in hel. basis ref. sample
    TH1F* hCosBeamLBeam_;          // pseudo-angle l/spin in beam basis
    TH1F* hCosBeamBBeam_;          // pseudo-angle b/spin in beam basis
    TH1F* hCosBeamQBeam_;          // pseudo-angle q/spin in beam basis
    TH1F* hAnaCosBeamLBeam_;       // pseudo-angle l/spin in beam basis ana. sample
    TH1F* hAnaCosBeamBBeam_;       // pseudo-angle b/spin in beam basis ana. sample
    TH1F* hAnaCosBeamQBeam_;       // pseudo-angle q/spin in beam basis ana. sample
    TH1F* hRefCosBeamLBeam_;       // pseudo-angle l/spin in beam basis ref. sample
    TH1F* hRefCosBeamBBeam_;       // pseudo-angle b/spin in beam basis ref. sample
    TH1F* hRefCosBeamQBeam_;       // pseudo-angle q/spin in beam basis ref. sample
    TH1F* hCosOffDiagLOffDiag_;    // pseudo-angle l/spin in off-diag. basis
    TH1F* hCosOffDiagBOffDiag_;    // pseudo-angle b/spin in off-diag. basis
    TH1F* hCosOffDiagQOffDiag_;    // pseudo-angle q/spin in off-diag. basis
    TH1F* hAnaCosOffDiagLOffDiag_; // pseudo-angle l/spin in off-diag. basis ana. sample
    TH1F* hAnaCosOffDiagBOffDiag_; // pseudo-angle b/spin in off-diag. basis ana. sample
    TH1F* hAnaCosOffDiagQOffDiag_; // pseudo-angle q/spin in off-diag. basis ana. sample
    TH1F* hRefCosOffDiagLOffDiag_; // pseudo-angle l/spin in off-diag. basis ref. sample
    TH1F* hRefCosOffDiagBOffDiag_; // pseudo-angle b/spin in off-diag. basis ref. sample
    TH1F* hRefCosOffDiagQOffDiag_; // pseudo-angle q/spin in off-diag. basis ref. sample
    TH1F* hCosLB_;    // pseudo-angle l/b
    TH1F* hCosLQ_;    // pseudo-angle l/q
    TH1F* hAnaCosLB_; // pseudo-angle l/b ana. sample
    TH1F* hAnaCosLQ_; // pseudo-angle l/q ana. sample
    TH1F* hRefCosLB_; // pseudo-angle l/b ref. sample
    TH1F* hRefCosLQ_; // pseudo-angle l/q ref. sample

    // specific histos
    TH2F* hCosTBCosTLHel_;                    // pseudo-angles l/t vs. b/t in hel. basis
    TH2F* hCosTQCosTLHel_;                    // pseudo-angles l/t vs. q/t in hel. basis
    TH2F* hCosBeamBCosBeamLBeam_;             // pseudo-angles l/t vs. b/t in beam basis
    TH2F* hCosBeamQCosBeamLBeam_;             // pseudo-angles l/t vs. q/t in beam basis
    TH2F* hCosOffDiagBCosOffDiagLOffDiag_;    // pseudo-angles l/t vs. b/t in off-diag. basis
    TH2F* hCosOffDiagQCosOffDiagLOffDiag_;    // pseudo-angles l/t vs. q/t in off-diag. basis
    TH2F* hCosTBCosTLOpt_;                    // pseudo-angles l/t vs. b/t in opt. basis
    TH2F* hCosTQCosTLOpt_;                    // pseudo-angles l/t vs. q/t in opt. basis
    TH2F* hAnaCosTBCosTLHel_;                 // pseudo-angles l/t vs. b/t in hel. basis ana. sample
    TH2F* hAnaCosTQCosTLHel_;                 // pseudo-angles l/t vs. q/t in hel. basis ana. sample
    TH2F* hAnaCosBeamBCosBeamLBeam_;          // pseudo-angles l/t vs. b/t in beam basis ana. sample
    TH2F* hAnaCosBeamQCosBeamLBeam_;          // pseudo-angles l/t vs. q/t in beam basis ana. sample
    TH2F* hAnaCosOffDiagBCosOffDiagLOffDiag_; // pseudo-angles l/t vs. b/t in off-diag. basis ana. sample
    TH2F* hAnaCosOffDiagQCosOffDiagLOffDiag_; // pseudo-angles l/t vs. q/t in off-diag. basis ana. sample
    TH2F* hAnaCosTBCosTLOpt_;                 // pseudo-angles l/t vs. b/t in opt. basis ana. sample
    TH2F* hAnaCosTQCosTLOpt_;                 // pseudo-angles l/t vs. q/t in opt. basis ana. sample
    TH2F* hRefCosTBCosTLHel_;                 // pseudo-angles l/t vs. b/t in hel. basis ref. sample
    TH2F* hRefCosTQCosTLHel_;                 // pseudo-angles l/t vs. q/t in hel. basis ref. sample
    TH2F* hRefCosBeamBCosBeamLBeam_;          // pseudo-angles l/t vs. b/t in beam basis ref. sample
    TH2F* hRefCosBeamQCosBeamLBeam_;          // pseudo-angles l/t vs. q/t in beam basis ref. sample
    TH2F* hRefCosOffDiagBCosOffDiagLOffDiag_; // pseudo-angles l/t vs. b/t in off-diag. basis ref. sample
    TH2F* hRefCosOffDiagQCosOffDiagLOffDiag_; // pseudo-angles l/t vs. q/t in off-diag. basis ref. sample
    TH2F* hRefCosTBCosTLOpt_;                 // pseudo-angles l/t vs. b/t in opt. basis ref. sample
    TH2F* hRefCosTQCosTLOpt_;                 // pseudo-angles l/t vs. q/t in opt. basis ref. sample

    // generic observables
    double topsZMFMass_;
    double splitTtMass_;
    double cosPhiLB_,
           cosPhiLQ_;

    // specific observables
    double cosThetaTLHel_,
           cosThetaTBHel_,
           cosThetaTQHel_;
    double cosThetaBeamLBeam_,
           cosThetaBeamBBeam_,
           cosThetaBeamQBeam_;
    double cosThetaOffDiagLOffDiag_,
           cosThetaOffDiagBOffDiag_,
           cosThetaOffDiagQOffDiag_;
    double cosThetaTLOpt_,
           cosThetaTBOpt_,
           cosThetaTQOpt_;

};


#endif
