#ifndef TtSpinCorrelations_AnalyzeGenEvent_h
#define TtSpinCorrelations_AnalyzeGenEvent_h


//
// Original Author:  Volker Adler
//         Created:  Fri Feb  8 14:48:21 CET 2008
// $Id: AnalyzeGenEvent.h,v 1.1 2008/02/13 15:04:27 vadler Exp $
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
#include "TH1D.h"
#include "TH2D.h"
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
    TH1D* hTtMass_; // tt mass
    TH1D* hCosLB_;    // pseudo-angle l/b
    TH1D* hCosLQ_;    // pseudo-angle l/q
    TH1D* hAnaCosLB_; // pseudo-angle l/b ana. sample
    TH1D* hAnaCosLQ_; // pseudo-angle l/q ana. sample
    TH1D* hRefCosLB_; // pseudo-angle l/b ref. sample
    TH1D* hRefCosLQ_; // pseudo-angle l/q ref. sample

    // specific histos
    TH1D* hCosTLHel_;              // pseudo-angle l/spin in hel. basis
    TH1D* hCosTBHel_;              // pseudo-angle b/spin in hel. basis
    TH1D* hCosTQHel_;              // pseudo-angle q/spin in hel. basis
    TH1D* hAnaCosTLHel_;           // pseudo-angle l/spin in hel. basis ana. sample
    TH1D* hAnaCosTBHel_;           // pseudo-angle b/spin in hel. basis ana. sample
    TH1D* hAnaCosTQHel_;           // pseudo-angle q/spin in hel. basis ana. sample
    TH1D* hRefCosTLHel_;           // pseudo-angle l/spin in hel. basis ref. sample
    TH1D* hRefCosTBHel_;           // pseudo-angle b/spin in hel. basis ref. sample
    TH1D* hRefCosTQHel_;           // pseudo-angle q/spin in hel. basis ref. sample
    TH1D* hCosBeamLBeam_;          // pseudo-angle l/spin in beam basis
    TH1D* hCosBeamBBeam_;          // pseudo-angle b/spin in beam basis
    TH1D* hCosBeamQBeam_;          // pseudo-angle q/spin in beam basis
    TH1D* hAnaCosBeamLBeam_;       // pseudo-angle l/spin in beam basis ana. sample
    TH1D* hAnaCosBeamBBeam_;       // pseudo-angle b/spin in beam basis ana. sample
    TH1D* hAnaCosBeamQBeam_;       // pseudo-angle q/spin in beam basis ana. sample
    TH1D* hRefCosBeamLBeam_;       // pseudo-angle l/spin in beam basis ref. sample
    TH1D* hRefCosBeamBBeam_;       // pseudo-angle b/spin in beam basis ref. sample
    TH1D* hRefCosBeamQBeam_;       // pseudo-angle q/spin in beam basis ref. sample
    TH1D* hCosOffDiagLOffDiag_;    // pseudo-angle l/spin in off-diag. basis
    TH1D* hCosOffDiagBOffDiag_;    // pseudo-angle b/spin in off-diag. basis
    TH1D* hCosOffDiagQOffDiag_;    // pseudo-angle q/spin in off-diag. basis
    TH1D* hAnaCosOffDiagLOffDiag_; // pseudo-angle l/spin in off-diag. basis ana. sample
    TH1D* hAnaCosOffDiagBOffDiag_; // pseudo-angle b/spin in off-diag. basis ana. sample
    TH1D* hAnaCosOffDiagQOffDiag_; // pseudo-angle q/spin in off-diag. basis ana. sample
    TH1D* hRefCosOffDiagLOffDiag_; // pseudo-angle l/spin in off-diag. basis ref. sample
    TH1D* hRefCosOffDiagBOffDiag_; // pseudo-angle b/spin in off-diag. basis ref. sample
    TH1D* hRefCosOffDiagQOffDiag_; // pseudo-angle q/spin in off-diag. basis ref. sample
    TH2D* hCosTBCosTLHel_;                    // pseudo-angles l/t vs. b/t in hel. basis
    TH2D* hCosTQCosTLHel_;                    // pseudo-angles l/t vs. q/t in hel. basis
    TH2D* hCosBeamBCosBeamLBeam_;             // pseudo-angles l/t vs. b/t in beam basis
    TH2D* hCosBeamQCosBeamLBeam_;             // pseudo-angles l/t vs. q/t in beam basis
    TH2D* hCosOffDiagBCosOffDiagLOffDiag_;    // pseudo-angles l/t vs. b/t in off-diag. basis
    TH2D* hCosOffDiagQCosOffDiagLOffDiag_;    // pseudo-angles l/t vs. q/t in off-diag. basis
    TH2D* hCosTBCosTLOpt_;                    // pseudo-angles l/t vs. b/t in opt. basis
    TH2D* hCosTQCosTLOpt_;                    // pseudo-angles l/t vs. q/t in opt. basis
    TH2D* hAnaCosTBCosTLHel_;                 // pseudo-angles l/t vs. b/t in hel. basis ana. sample
    TH2D* hAnaCosTQCosTLHel_;                 // pseudo-angles l/t vs. q/t in hel. basis ana. sample
    TH2D* hAnaCosBeamBCosBeamLBeam_;          // pseudo-angles l/t vs. b/t in beam basis ana. sample
    TH2D* hAnaCosBeamQCosBeamLBeam_;          // pseudo-angles l/t vs. q/t in beam basis ana. sample
    TH2D* hAnaCosOffDiagBCosOffDiagLOffDiag_; // pseudo-angles l/t vs. b/t in off-diag. basis ana. sample
    TH2D* hAnaCosOffDiagQCosOffDiagLOffDiag_; // pseudo-angles l/t vs. q/t in off-diag. basis ana. sample
    TH2D* hAnaCosTBCosTLOpt_;                 // pseudo-angles l/t vs. b/t in opt. basis ana. sample
    TH2D* hAnaCosTQCosTLOpt_;                 // pseudo-angles l/t vs. q/t in opt. basis ana. sample
    TH2D* hRefCosTBCosTLHel_;                 // pseudo-angles l/t vs. b/t in hel. basis ref. sample
    TH2D* hRefCosTQCosTLHel_;                 // pseudo-angles l/t vs. q/t in hel. basis ref. sample
    TH2D* hRefCosBeamBCosBeamLBeam_;          // pseudo-angles l/t vs. b/t in beam basis ref. sample
    TH2D* hRefCosBeamQCosBeamLBeam_;          // pseudo-angles l/t vs. q/t in beam basis ref. sample
    TH2D* hRefCosOffDiagBCosOffDiagLOffDiag_; // pseudo-angles l/t vs. b/t in off-diag. basis ref. sample
    TH2D* hRefCosOffDiagQCosOffDiagLOffDiag_; // pseudo-angles l/t vs. q/t in off-diag. basis ref. sample
    TH2D* hRefCosTBCosTLOpt_;                 // pseudo-angles l/t vs. b/t in opt. basis ref. sample
    TH2D* hRefCosTQCosTLOpt_;                 // pseudo-angles l/t vs. q/t in opt. basis ref. sample
    // Clones of TH2 (display problems)
    TH2D* hcCosTBCosTLHel_;
    TH2D* hcCosTQCosTLHel_;                   
    TH2D* hcCosBeamBCosBeamLBeam_;            
    TH2D* hcCosBeamQCosBeamLBeam_;            
    TH2D* hcCosOffDiagBCosOffDiagLOffDiag_;   
    TH2D* hcCosOffDiagQCosOffDiagLOffDiag_;   
    TH2D* hcCosTBCosTLOpt_;                   
    TH2D* hcCosTQCosTLOpt_;                   
    TH2D* hcAnaCosTBCosTLHel_;                
    TH2D* hcAnaCosTQCosTLHel_;                
    TH2D* hcAnaCosBeamBCosBeamLBeam_;         
    TH2D* hcAnaCosBeamQCosBeamLBeam_;         
    TH2D* hcAnaCosOffDiagBCosOffDiagLOffDiag_;
    TH2D* hcAnaCosOffDiagQCosOffDiagLOffDiag_;
    TH2D* hcAnaCosTBCosTLOpt_;                
    TH2D* hcAnaCosTQCosTLOpt_;                
    TH2D* hcRefCosTBCosTLHel_;                
    TH2D* hcRefCosTQCosTLHel_;                
    TH2D* hcRefCosBeamBCosBeamLBeam_;         
    TH2D* hcRefCosBeamQCosBeamLBeam_;         
    TH2D* hcRefCosOffDiagBCosOffDiagLOffDiag_;
    TH2D* hcRefCosOffDiagQCosOffDiagLOffDiag_;
    TH2D* hcRefCosTBCosTLOpt_;                
    TH2D* hcRefCosTQCosTLOpt_;

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
