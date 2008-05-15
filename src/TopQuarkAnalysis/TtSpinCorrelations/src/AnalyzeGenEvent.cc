
// -*- C++ -*-
//
// Package:    TtSpinCorrelations
// Class:      AnalyzeGenEvent
//
/**\class AnalyzeGenEvent AnalyzeGenEvent.cc TopQuarkAnalysis/TtSpinCorrelations/src/AnalyzeGenEvent.cc

 Description: This class analyzes a GenEvent in the context of ttbar spin correlations.

 Implementation:
     No documentation yet.
*/
//
// Original Author:  Volker Adler
//         Created:  Fri Feb  8 14:48:21 CET 2008
// $Id: AnalyzeGenEvent.cc,v 1.2 2008/02/16 19:11:58 vadler Exp $
//


#include "TopQuarkAnalysis/TtSpinCorrelations/interface/AnalyzeGenEvent.h"


AnalyzeGenEvent::AnalyzeGenEvent(const edm::ParameterSet& iConfig) :
  // initialize
  useHelBasis_    (iConfig.getParameter<bool>         ("useHelicityBasis")),
  useBeamBasis_   (iConfig.getParameter<bool>         ("useBeamBasis")),
  useOffDiagBasis_(iConfig.getParameter<bool>         ("useOffDiagonalBasis")),
  useOptBasis_    (iConfig.getParameter<bool>         ("useOptimalBasis")),
  splitTtAbsolute_(iConfig.getParameter<bool>         ("splitTtMassAbsolute")),
  fitHistos_      (iConfig.getParameter<bool>         ("fitHistos")),
  srcGenEvt_      (iConfig.getParameter<edm::InputTag>("srcGenEvent")),
  filePath_       (iConfig.getParameter<std::string>  ("outputFilePath")),
  splitTtValue_   (iConfig.getParameter<double>       ("splitTtValue")),
  binsTtMass_     (iConfig.getParameter<int>          ("binsTtMass")),
  xMinTtMass_     (iConfig.getParameter<double>       ("xMinTtMass")),
  xMaxTtMass_     (iConfig.getParameter<double>       ("xMaxTtMass")),
  binsCosAngle1D_ (iConfig.getParameter<int>          ("binsCosAngle1D")),
  binsCosAngle2D_ (iConfig.getParameter<int>          ("binsCosAngle2D")),
  kappaLplus_     (iConfig.getParameter<double>       ("kappaLplus")),
  kappaDbar_      (iConfig.getParameter<double>       ("kappaDbar")),
  kappaSbar_      (iConfig.getParameter<double>       ("kappaSbar")),
  kappaNu_        (iConfig.getParameter<double>       ("kappaNu")),
  kappaU_         (iConfig.getParameter<double>       ("kappaU")),
  kappaC_         (iConfig.getParameter<double>       ("kappaC")),
  kappaB_         (iConfig.getParameter<double>       ("kappaB")),
  kappaW_         (iConfig.getParameter<double>       ("kappaW")),
  kappaQ_         (iConfig.getParameter<double>       ("kappaQ"))
{
  // create output file
  file_ = new TFile(filePath_.data(), "RECREATE");

  // define trees & histograms
  tree_ = new TTree("tree", "Spin basis independent observables (tt ZMF)");
  tree_->Branch("topsZMFMass_",     &topsZMFMass_,     "topsZMFMass_/D");
  tree_->Branch("cosPhiLB_",        &cosPhiLB_,        "cosPhiLB_/D");
  tree_->Branch("cosPhiLQ_",        &cosPhiLQ_,        "cosPhiLQ_/D");
  treeMeta_ = new TTree("treeMeta", "Event and spin independent observables");
  treeMeta_->Branch("splitTtAbsolute_", &splitTtAbsolute_, "splitTtAbsolute_/b");
  treeMeta_->Branch("splitTtValue_",    &splitTtValue_,    "splitTtValue_/D");
  treeMeta_->Branch("splitTtMass_",     &splitTtMass_,     "splitTtMass_/D");
  treeMeta_->Branch("kappaLplus_",      &kappaLplus_,      "kappaLplus_/D");
  treeMeta_->Branch("kappaDbar_",       &kappaDbar_,       "kappaDbar_/D");
  treeMeta_->Branch("kappaSbar_",       &kappaSbar_,       "kappaSbar_/D");
  treeMeta_->Branch("kappaNu_",         &kappaNu_,         "kappaNu_/D");
  treeMeta_->Branch("kappaU_",          &kappaU_,          "kappaU_/D");
  treeMeta_->Branch("kappaC_",          &kappaC_,          "kappaC_/D");
  treeMeta_->Branch("kappaB_",          &kappaB_,          "kappaB_/D");
  treeMeta_->Branch("kappaW_",          &kappaW_,          "kappaW_/D");
  treeMeta_->Branch("kappaQ_",          &kappaQ_,          "kappaQ_/D");
  hTtMass_ = new TH1D("hTtMass", "t#bar{t} invariant mass", binsTtMass_, xMinTtMass_, xMaxTtMass_);
  hTtMass_->SetXTitle("m_{t#bar{t}} (GeV)");
  hTtMass_->SetYTitle("events");
  hCosLB_ = new TH1D("hCosLB_", "Angle between lepton and b-quark", binsCosAngle1D_, -1., 1.);
  hCosLB_->SetXTitle("cos #phi_{l,b}");
  hCosLB_->SetXTitle("events");
  hCosLQ_ = new TH1D("hCosLQ_", "Angle between lepton and low-energy quark", binsCosAngle1D_, -1., 1.);
  hCosLQ_->SetXTitle("cos #phi_{l,q}");
  hCosLQ_->SetXTitle("events");
  hAnaCosLB_ = new TH1D("hAnaCosLB_", "Angle between lepton and b-quark (analysis)", binsCosAngle1D_, -1., 1.);
  hAnaCosLB_->SetXTitle("cos #phi_{l,b}");
  hAnaCosLB_->SetXTitle("events");
  hAnaCosLQ_ = new TH1D("hAnaCosLQ_", "Angle between lepton and low-energy quark (analysis)", binsCosAngle1D_, -1., 1.);
  hAnaCosLQ_->SetXTitle("cos #phi_{l,q}");
  hAnaCosLQ_->SetXTitle("events");
  hRefCosLB_ = new TH1D("hRefCosLB_", "Angle between lepton and b-quark (reference)", binsCosAngle1D_, -1., 1.);
  hRefCosLB_->SetXTitle("cos #phi_{l,b}");
  hRefCosLB_->SetXTitle("events");
  hRefCosLQ_ = new TH1D("hRefCosLQ_", "Angle between lepton and low-energy quark (reference)", binsCosAngle1D_, -1., 1.);
  hRefCosLQ_->SetXTitle("cos #phi_{l,q}");
  hRefCosLQ_->SetXTitle("events");
  if ( useHelBasis_ ) {
    file_->mkdir("basisHelicity");
    treeHel_ = new TTree("treeHel", "Observables in helicity basis");
    treeHel_->Branch("cosThetaTLHel_", &cosThetaTLHel_, "cosThetaTLHel_/D");
    treeHel_->Branch("cosThetaTBHel_", &cosThetaTBHel_, "cosThetaTBHel_/D");
    treeHel_->Branch("cosThetaTQHel_", &cosThetaTQHel_, "cosThetaTQHel_/D");
    hCosTLHel_ = new TH1D("hCosTLHel_", "Pseudo-angle between t-quark and lepton", binsCosAngle1D_, -1., 1.);
    hCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hCosTLHel_->SetYTitle("events");
    hCosTBHel_ = new TH1D("hCosTBHel_", "Pseudo-angle between t-quark and b-quark", binsCosAngle1D_, -1., 1.);
    hCosTBHel_->SetXTitle("cos #theta_{t,b}");
    hCosTBHel_->SetYTitle("events");
    hCosTQHel_ = new TH1D("hCosTQHel_", "Pseudo-angle between t-quark and low-energy quark", binsCosAngle1D_, -1., 1.);
    hCosTQHel_->SetXTitle("cos #theta_{t,q}");
    hCosTQHel_->SetYTitle("events");
    hAnaCosTLHel_ = new TH1D("hAnaCosTLHel_", "Pseudo-angle between t-quark and lepton (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hAnaCosTLHel_->SetYTitle("events");
    hAnaCosTBHel_ = new TH1D("hAnaCosTBHel_", "Pseudo-angle between t-quark and b-quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosTBHel_->SetXTitle("cos #theta_{t,b}");
    hAnaCosTBHel_->SetYTitle("events");
    hAnaCosTQHel_ = new TH1D("hAnaCosTQHel_", "Pseudo-angle between t-quark and low-energy quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosTQHel_->SetXTitle("cos #theta_{t,q}");
    hAnaCosTQHel_->SetYTitle("events");
    hRefCosTLHel_ = new TH1D("hRefCosTLHel_", "Pseudo-angle between t-quark and lepton (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hRefCosTLHel_->SetYTitle("events");
    hRefCosTBHel_ = new TH1D("hRefCosTBHel_", "Pseudo-angle between t-quark and b-quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosTBHel_->SetXTitle("cos #theta_{t,b}");
    hRefCosTBHel_->SetYTitle("events");
    hRefCosTQHel_ = new TH1D("hRefCosTQHel_", "Pseudo-angle between t-quark and low-energy quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosTQHel_->SetXTitle("cos #theta_{t,q}");
    hRefCosTQHel_->SetYTitle("events");
    hCosTBCosTLHel_ = new TH2D("hCosTBCosTLHel_", "Pseudo-angles between t-quark and lepton/b-quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosTBCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hCosTBCosTLHel_->SetYTitle("cos #theta_{t,b}");
    hCosTBCosTLHel_->SetZTitle("events");
    hCosTQCosTLHel_ = new TH2D("hCosTQCosTLHel_", "Pseudo-angles between t-quark and lepton/low-energy quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosTQCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hCosTQCosTLHel_->SetYTitle("cos #theta_{t,q}");
    hCosTQCosTLHel_->SetZTitle("events");
    hAnaCosTBCosTLHel_ = new TH2D("hAnaCosTBCosTLHel_", "Pseudo-angles between t-quark and lepton/b-quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosTBCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hAnaCosTBCosTLHel_->SetYTitle("cos #theta_{t,b}");
    hAnaCosTBCosTLHel_->SetZTitle("events");
    hAnaCosTQCosTLHel_ = new TH2D("hAnaCosTQCosTLHel_", "Pseudo-angles between t-quark and lepton/low-energy quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosTQCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hAnaCosTQCosTLHel_->SetYTitle("cos #theta_{t,q}");
    hAnaCosTQCosTLHel_->SetZTitle("events");
    hRefCosTBCosTLHel_ = new TH2D("hRefCosTBCosTLHel_", "Pseudo-angles between t-quark and lepton/b-quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosTBCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hRefCosTBCosTLHel_->SetYTitle("cos #theta_{t,b}");
    hRefCosTBCosTLHel_->SetZTitle("events");
    hRefCosTQCosTLHel_ = new TH2D("hRefCosTQCosTLHel_", "Pseudo-angles between t-quark and lepton/low-energy quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosTQCosTLHel_->SetXTitle("cos #theta_{t,l}");
    hRefCosTQCosTLHel_->SetYTitle("cos #theta_{t,q}");
    hRefCosTQCosTLHel_->SetZTitle("events");
    file_->cd();
  }
  if ( useBeamBasis_ ) {
    file_->mkdir("basisBeam");
    treeBeam_ = new TTree("treeBeam", "Observables in beam basis");
    treeBeam_->Branch("cosThetaBeamLBeam_", &cosThetaBeamLBeam_, "cosThetaBeamLBeam_/D");
    treeBeam_->Branch("cosThetaBeamBBeam_", &cosThetaBeamBBeam_, "cosThetaBeamBBeam_/D");
    treeBeam_->Branch("cosThetaBeamQBeam_", &cosThetaBeamQBeam_, "cosThetaBeamQBeam_/D");
    hCosBeamLBeam_ = new TH1D("hCosBeamLBeam_", "Pseudo-angle between initial parton and lepton", binsCosAngle1D_, -1., 1.);
    hCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hCosBeamLBeam_->SetYTitle("events");
    hCosBeamBBeam_ = new TH1D("hCosBeamBBeam_", "Pseudo-angle between initial parton and b-quark", binsCosAngle1D_, -1., 1.);
    hCosBeamBBeam_->SetXTitle("cos #theta_{ini,b}");
    hCosBeamBBeam_->SetYTitle("events");
    hCosBeamQBeam_ = new TH1D("hCosBeamQBeam_", "Pseudo-angle between initial parton and low-energy quark", binsCosAngle1D_, -1., 1.);
    hCosBeamQBeam_->SetXTitle("cos #theta_{ini,q}");
    hCosBeamQBeam_->SetYTitle("events");
    hAnaCosBeamLBeam_ = new TH1D("hAnaCosBeamLBeam_", "Pseudo-angle between initial parton and lepton (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hAnaCosBeamLBeam_->SetYTitle("events");
    hAnaCosBeamBBeam_ = new TH1D("hAnaCosBeamBBeam_", "Pseudo-angle between initial parton and b-quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosBeamBBeam_->SetXTitle("cos #theta_{ini,b}");
    hAnaCosBeamBBeam_->SetYTitle("events");
    hAnaCosBeamQBeam_ = new TH1D("hAnaCosBeamQBeam_", "Pseudo-angle between initial parton and low-energy quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosBeamQBeam_->SetXTitle("cos #theta_{ini,q}");
    hAnaCosBeamQBeam_->SetYTitle("events");
    hRefCosBeamLBeam_ = new TH1D("hRefCosBeamLBeam_", "Pseudo-angle between initial parton and lepton (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hRefCosBeamLBeam_->SetYTitle("events");
    hRefCosBeamBBeam_ = new TH1D("hRefCosBeamBBeam_", "Pseudo-angle between initial parton and b-quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosBeamBBeam_->SetXTitle("cos #theta_{ini,b}");
    hRefCosBeamBBeam_->SetYTitle("events");
    hRefCosBeamQBeam_ = new TH1D("hRefCosBeamQBeam_", "Pseudo-angle between initial parton and low-energy quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosBeamQBeam_->SetXTitle("cos #theta_{ini,q}");
    hRefCosBeamQBeam_->SetYTitle("events");
    hCosBeamBCosBeamLBeam_ = new TH2D("hCosBeamBCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/b-quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosBeamBCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hCosBeamBCosBeamLBeam_->SetYTitle("cos #theta_{ini,b}");
    hCosBeamBCosBeamLBeam_->SetZTitle("events");
    hCosBeamQCosBeamLBeam_ = new TH2D("hCosBeamQCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/low-energy quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosBeamQCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hCosBeamQCosBeamLBeam_->SetYTitle("cos #theta_{ini,q}");
    hCosBeamQCosBeamLBeam_->SetZTitle("events");
    hAnaCosBeamBCosBeamLBeam_ = new TH2D("hAnaCosBeamBCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/b-quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosBeamBCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hAnaCosBeamBCosBeamLBeam_->SetYTitle("cos #theta_{ini,b}");
    hAnaCosBeamBCosBeamLBeam_->SetZTitle("events");
    hAnaCosBeamQCosBeamLBeam_ = new TH2D("hAnaCosBeamQCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/low-energy quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosBeamQCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hAnaCosBeamQCosBeamLBeam_->SetYTitle("cos #theta_{ini,q}");
    hAnaCosBeamQCosBeamLBeam_->SetZTitle("events");
    hRefCosBeamBCosBeamLBeam_ = new TH2D("hRefCosBeamBCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/b-quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosBeamBCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hRefCosBeamBCosBeamLBeam_->SetYTitle("cos #theta_{ini,b}");
    hRefCosBeamBCosBeamLBeam_->SetZTitle("events");
    hRefCosBeamQCosBeamLBeam_ = new TH2D("hRefCosBeamQCosBeamLBeam_", "Pseudo-angles between initial parton and lepton/low-energy quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosBeamQCosBeamLBeam_->SetXTitle("cos #theta_{ini,l}");
    hRefCosBeamQCosBeamLBeam_->SetYTitle("cos #theta_{ini,q}");
    hRefCosBeamQCosBeamLBeam_->SetZTitle("events");
    file_->cd();
  }
  if ( useOffDiagBasis_ ) {
    file_->mkdir("basisOffDiag");
    treeOffDiag_ = new TTree("treeOffDiag", "Observables in off-diagonal basis");
    treeOffDiag_->Branch("cosThetaOffDiagLOffDiag_", &cosThetaOffDiagLOffDiag_, "cosThetaOffDiagLOffDiag_/D");
    treeOffDiag_->Branch("cosThetaOffDiagBOffDiag_", &cosThetaOffDiagBOffDiag_, "cosThetaOffDiagBOffDiag_/D");
    treeOffDiag_->Branch("cosThetaOffDiagQOffDiag_", &cosThetaOffDiagQOffDiag_, "cosThetaOffDiagQOffDiag_/D");
    hCosOffDiagLOffDiag_ = new TH1D("hCosOffDiagLOffDiag_", "Pseudo-angle between off-diag. basis and lepton", binsCosAngle1D_, -1., 1.);
    hCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hCosOffDiagLOffDiag_->SetYTitle("events");
    hCosOffDiagBOffDiag_ = new TH1D("hCosOffDiagBOffDiag_", "Pseudo-angle between off-diag. basis and b-quark", binsCosAngle1D_, -1., 1.);
    hCosOffDiagBOffDiag_->SetXTitle("cos #theta_{off,b}");
    hCosOffDiagBOffDiag_->SetYTitle("events");
    hCosOffDiagQOffDiag_ = new TH1D("hCosOffDiagQOffDiag_", "Pseudo-angle between off-diag. basis and low-energy quark", binsCosAngle1D_, -1., 1.);
    hCosOffDiagQOffDiag_->SetXTitle("cos #theta_{off,q}");
    hCosOffDiagQOffDiag_->SetYTitle("events");
    hAnaCosOffDiagLOffDiag_ = new TH1D("hAnaCosOffDiagLOffDiag_", "Pseudo-angle between off-diag. basis and lepton (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hAnaCosOffDiagLOffDiag_->SetYTitle("events");
    hAnaCosOffDiagBOffDiag_ = new TH1D("hAnaCosOffDiagBOffDiag_", "Pseudo-angle between off-diag. basis and b-quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosOffDiagBOffDiag_->SetXTitle("cos #theta_{off,b}");
    hAnaCosOffDiagBOffDiag_->SetYTitle("events");
    hAnaCosOffDiagQOffDiag_ = new TH1D("hAnaCosOffDiagQOffDiag_", "Pseudo-angle between off-diag. basis and low-energy quark (analysis)", binsCosAngle1D_, -1., 1.);
    hAnaCosOffDiagQOffDiag_->SetXTitle("cos #theta_{off,q}");
    hAnaCosOffDiagQOffDiag_->SetYTitle("events");
    hRefCosOffDiagLOffDiag_ = new TH1D("hRefCosOffDiagLOffDiag_", "Pseudo-angle between off-diag. basis and lepton (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hRefCosOffDiagLOffDiag_->SetYTitle("events");
    hRefCosOffDiagBOffDiag_ = new TH1D("hRefCosOffDiagBOffDiag_", "Pseudo-angle between off-diag. basis and b-quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosOffDiagBOffDiag_->SetXTitle("cos #theta_{off,b}");
    hRefCosOffDiagBOffDiag_->SetYTitle("events");
    hRefCosOffDiagQOffDiag_ = new TH1D("hRefCosOffDiagQOffDiag_", "Pseudo-angle between off-diag. basis and low-energy quark (reference)", binsCosAngle1D_, -1., 1.);
    hRefCosOffDiagQOffDiag_->SetXTitle("cos #theta_{off,q}");
    hRefCosOffDiagQOffDiag_->SetYTitle("events");
    hCosOffDiagBCosOffDiagLOffDiag_ = new TH2D("hCosOffDiagBCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/b-quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosOffDiagBCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hCosOffDiagBCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,b}");
    hCosOffDiagBCosOffDiagLOffDiag_->SetZTitle("events");
    hCosOffDiagQCosOffDiagLOffDiag_ = new TH2D("hCosOffDiagQCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/low-energy quark", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hCosOffDiagQCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hCosOffDiagQCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,q}");
    hCosOffDiagQCosOffDiagLOffDiag_->SetZTitle("events");
    hAnaCosOffDiagBCosOffDiagLOffDiag_ = new TH2D("hAnaCosOffDiagBCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/b-quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosOffDiagBCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hAnaCosOffDiagBCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,b}");
    hAnaCosOffDiagBCosOffDiagLOffDiag_->SetZTitle("events");
    hAnaCosOffDiagQCosOffDiagLOffDiag_ = new TH2D("hAnaCosOffDiagQCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/low-energy quark (analysis)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hAnaCosOffDiagQCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hAnaCosOffDiagQCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,q}");
    hAnaCosOffDiagQCosOffDiagLOffDiag_->SetZTitle("events");
    hRefCosOffDiagBCosOffDiagLOffDiag_ = new TH2D("hRefCosOffDiagBCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/b-quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosOffDiagBCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hRefCosOffDiagBCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,b}");
    hRefCosOffDiagBCosOffDiagLOffDiag_->SetZTitle("events");
    hRefCosOffDiagQCosOffDiagLOffDiag_ = new TH2D("hRefCosOffDiagQCosOffDiagLOffDiag_", "Pseudo-angles between off-diag. basis and lepton/low-energy quark (reference)", binsCosAngle2D_, -1., 1., binsCosAngle2D_, -1., 1.);
    hRefCosOffDiagQCosOffDiagLOffDiag_->SetXTitle("cos #theta_{off,l}");
    hRefCosOffDiagQCosOffDiagLOffDiag_->SetYTitle("cos #theta_{off,q}");
    hRefCosOffDiagQCosOffDiagLOffDiag_->SetZTitle("events");
    file_->cd();
  }
  if ( fitHistos_ ) {
    fit1L_ = TF1("fit1L_", "[0]*(1.+[1]*[2]*x)");
    fit1L_.SetParameter(2,kappaLplus_);
    fit1L_.FixParameter(2,kappaLplus_);
    fit1B_ = TF1("fit1B_", "[0]*(1.+[1]*[2]*x)");
    fit1B_.SetParameter(2,kappaB_);
    fit1B_.FixParameter(2,kappaB_);
    fit1Q_ = TF1("fit1Q_", "[0]*(1.+[1]*[2]*x)");
    fit1Q_.SetParameter(2,kappaQ_);
    fit1Q_.FixParameter(2,kappaQ_);
    fit2LB_ = TF2("fit2LB_", "[0]*(1.-[1]*[2]*[3]*x*y)");
    fit2LB_.SetParameter(2,kappaLplus_);
    fit2LB_.FixParameter(2,kappaLplus_);
    fit2LB_.SetParameter(3,kappaB_);
    fit2LB_.FixParameter(3,kappaB_);
    fit2LQ_ = TF2("fit2LQ_", "[0]*(1.-[1]*[2]*[3]*x*y)");
    fit2LQ_.SetParameter(2,kappaLplus_);
    fit2LQ_.FixParameter(2,kappaLplus_);
    fit2LQ_.SetParameter(3,kappaQ_);
    fit2LQ_.FixParameter(3,kappaQ_);
  }
}


AnalyzeGenEvent::~AnalyzeGenEvent()
{
  // delete trees & histograms
  delete tree_;
  delete treeMeta_;
  delete hTtMass_;
  delete hCosLB_;
  delete hCosLQ_;
  delete hAnaCosLB_;
  delete hAnaCosLQ_;
  delete hRefCosLB_;
  delete hRefCosLQ_;
  if ( useHelBasis_ ) {
    file_->cd("basisHelicity");
    delete treeHel_;
    delete hCosTLHel_;
    delete hCosTBHel_;
    delete hCosTQHel_;
    delete hAnaCosTLHel_;
    delete hAnaCosTBHel_;
    delete hAnaCosTQHel_;
    delete hRefCosTLHel_;
    delete hRefCosTBHel_;
    delete hRefCosTQHel_;
    delete hCosTBCosTLHel_;
    delete hCosTQCosTLHel_;
    delete hAnaCosTBCosTLHel_;
    delete hAnaCosTQCosTLHel_;
    delete hRefCosTBCosTLHel_;
    delete hRefCosTQCosTLHel_;
    file_->cd();
  }
  if ( useBeamBasis_ ) {
    file_->cd("basisBeam");
    delete treeBeam_;
    delete hCosBeamLBeam_;
    delete hCosBeamBBeam_;
    delete hCosBeamQBeam_;
    delete hAnaCosBeamLBeam_;
    delete hAnaCosBeamBBeam_;
    delete hAnaCosBeamQBeam_;
    delete hRefCosBeamLBeam_;
    delete hRefCosBeamBBeam_;
    delete hRefCosBeamQBeam_;
    delete hCosBeamBCosBeamLBeam_;
    delete hCosBeamQCosBeamLBeam_;
    delete hAnaCosBeamBCosBeamLBeam_;
    delete hAnaCosBeamQCosBeamLBeam_;
    delete hRefCosBeamBCosBeamLBeam_;
    delete hRefCosBeamQCosBeamLBeam_;
    file_->cd();
  }
  if ( useOffDiagBasis_ ) {
    file_->cd("basisOffDiag");
    delete treeOffDiag_;
    delete hCosOffDiagLOffDiag_;
    delete hCosOffDiagBOffDiag_;    
    delete hCosOffDiagQOffDiag_;    
    delete hAnaCosOffDiagLOffDiag_; 
    delete hAnaCosOffDiagBOffDiag_; 
    delete hAnaCosOffDiagQOffDiag_; 
    delete hRefCosOffDiagLOffDiag_; 
    delete hRefCosOffDiagBOffDiag_; 
    delete hRefCosOffDiagQOffDiag_;
    delete hCosOffDiagBCosOffDiagLOffDiag_;
    delete hCosOffDiagQCosOffDiagLOffDiag_;
    delete hAnaCosOffDiagBCosOffDiagLOffDiag_;
    delete hAnaCosOffDiagQCosOffDiagLOffDiag_;
    delete hRefCosOffDiagBCosOffDiagLOffDiag_;
    delete hRefCosOffDiagQCosOffDiagLOffDiag_;
    file_->cd();
  }

  // close and delete output file
  file_->Close();
  delete file_;
}


void AnalyzeGenEvent::beginJob(const edm::EventSetup& iSetup)
{
  // initialize meta observables (just for safety)
  splitTtMass_ = -99.;
}


/// Analyzer to read TtGenEvent and... [FIXME: add further info here.]
void AnalyzeGenEvent::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  // initialize observables (just for safety)
  topsZMFMass_ = -99.;
  cosPhiLB_                = -99.;
  cosPhiLQ_                = -99.;
  cosThetaTLHel_           = -99.;
  cosThetaTBHel_           = -99.;
  cosThetaTQHel_           = -99.;
  cosThetaBeamLBeam_       = -99.;
  cosThetaBeamBBeam_       = -99.;
  cosThetaBeamQBeam_       = -99.;
  cosThetaOffDiagLOffDiag_ = -99.;
  cosThetaOffDiagBOffDiag_ = -99.;
  cosThetaOffDiagQOffDiag_ = -99.;
  cosThetaTLOpt_           = -99.;
  cosThetaTBOpt_           = -99.;
  cosThetaTQOpt_           = -99.;

  // get the event
  edm::Handle<TtGenEvent> genEvt;
  iEvent.getByLabel(srcGenEvt_, genEvt);

  // compute only semi-leptonic events (at the moment)
  if ( ! genEvt->isSemiLeptonic() ) throw cms::Exception("TopGenEvent_notSemiLeptonic") << "The number of leptons is " << genEvt->numberOfLeptons() << " instead of 1!\n";

  // build CM sytems
  reco::Particle::LorentzVector topsZMF(genEvt->top()->p4()+genEvt->topBar()->p4());

  // boost particle 4-vectors to tt ZMF
  reco::Particle::LorentzVector tLeptonicZMF(ROOT::Math::VectorUtil::boost(genEvt->leptonicDecayTop()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector tHadronicZMF(ROOT::Math::VectorUtil::boost(genEvt->hadronicDecayTop()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector lLeptonicZMF(ROOT::Math::VectorUtil::boost(genEvt->singleLepton()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector bHadronicZMF(ROOT::Math::VectorUtil::boost(genEvt->hadronicDecayB()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector q1HadronicZMF(ROOT::Math::VectorUtil::boost(genEvt->hadronicDecayQuark()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector q2HadronicZMF(ROOT::Math::VectorUtil::boost(genEvt->hadronicDecayQuarkBar()->p4(), topsZMF.BoostToCM()));
  reco::Particle::LorentzVector beamZMF(ROOT::Math::VectorUtil::boost((genEvt->initialPartons())[0].p4(), topsZMF.BoostToCM())); // only one particle direction needed for beam basis
                                                                                                                                 // FIXME: use initial hadron instead of initial parton

  // build spin basis unit vectors
  reco::Particle::Vector leptHelZMF(tLeptonicZMF.Vect().Unit()),
                         hadrHelZMF(tHadronicZMF.Vect().Unit()); // = -leptHelZMF
  reco::Particle::Vector beamBeamZMF(beamZMF.Vect().Unit());
  reco::Particle::Vector offDiagZMF(((-beamBeamZMF+(1.-topsZMF.Gamma())*(beamBeamZMF.Dot(leptHelZMF))*leptHelZMF)/(std::sqrt(1.-std::pow(beamBeamZMF.Dot(leptHelZMF),2.)*(1.-std::pow(topsZMF.Gamma(),2.))))).Unit());                                                                                                                            // FIXME: use initial hadron instead of initial parton

  // boost 4-vectors to t(bar) rest frames
  reco::Particle::LorentzVector lLeptonicTRest(ROOT::Math::VectorUtil::boost(lLeptonicZMF, tLeptonicZMF.BoostToCM()));
  reco::Particle::LorentzVector bHadronicTRest(ROOT::Math::VectorUtil::boost(bHadronicZMF, tHadronicZMF.BoostToCM()));
  reco::Particle::LorentzVector q1HadronicTRest(ROOT::Math::VectorUtil::boost(q1HadronicZMF, tHadronicZMF.BoostToCM()));
  reco::Particle::LorentzVector q2HadronicTRest(ROOT::Math::VectorUtil::boost(q2HadronicZMF, tHadronicZMF.BoostToCM()));
  reco::Particle::LorentzVector qHadronicTRest(q1HadronicTRest.energy() < q2HadronicTRest.energy() ? // only lower energy quark used
                                               q1HadronicTRest :
                                               q2HadronicTRest);

  // extract particle directions in t(bar) rest frames
  reco::Particle::Vector lDirectionTRest(lLeptonicTRest.Vect().Unit());
  reco::Particle::Vector bDirectionTRest(bHadronicTRest.Vect().Unit());
  reco::Particle::Vector qDirectionTRest(qHadronicTRest.Vect().Unit());

  // compute observables
  topsZMFMass_ = topsZMF.mass();
  cosPhiLB_    = lDirectionTRest.Dot(bDirectionTRest);
  cosPhiLQ_    = lDirectionTRest.Dot(qDirectionTRest);
  if ( useHelBasis_ ) {
    cosThetaTLHel_ = leptHelZMF.Dot(lDirectionTRest);
    cosThetaTBHel_ = hadrHelZMF.Dot(bDirectionTRest);
    cosThetaTQHel_ = hadrHelZMF.Dot(qDirectionTRest);
  }
  if ( useBeamBasis_ ) {
    cosThetaBeamLBeam_ = beamBeamZMF.Dot(lDirectionTRest);
    cosThetaBeamBBeam_ = beamBeamZMF.Dot(bDirectionTRest);
    cosThetaBeamQBeam_ = beamBeamZMF.Dot(qDirectionTRest);
  }
  if ( useOffDiagBasis_ ) {
    cosThetaOffDiagLOffDiag_ = offDiagZMF.Dot(lDirectionTRest);
    cosThetaOffDiagBOffDiag_ = offDiagZMF.Dot(bDirectionTRest);
    cosThetaOffDiagQOffDiag_ = offDiagZMF.Dot(qDirectionTRest);
  }

  // fill trees & histograms
  file_->cd();
  tree_->Fill();
  hTtMass_->Fill(topsZMFMass_);
  hCosLB_ ->Fill(cosPhiLB_);
  hCosLQ_ ->Fill(cosPhiLQ_);
  if ( useHelBasis_ ) {
    file_->cd("basisHelicity");
    treeHel_->Fill();
    hCosTLHel_->Fill(cosThetaTLHel_);
    hCosTBHel_->Fill(cosThetaTBHel_);
    hCosTQHel_->Fill(cosThetaTQHel_);
    hCosTBCosTLHel_->Fill(cosThetaTBHel_, cosThetaTLHel_);
    hCosTQCosTLHel_->Fill(cosThetaTQHel_, cosThetaTLHel_);
    file_->cd();
  }
  if ( useBeamBasis_ ) {
    file_->cd("basisBeam");
    treeBeam_->Fill();
    hCosBeamLBeam_->Fill(cosThetaBeamLBeam_);
    hCosBeamBBeam_->Fill(cosThetaBeamBBeam_);
    hCosBeamQBeam_->Fill(cosThetaBeamQBeam_);
    hCosBeamBCosBeamLBeam_->Fill(cosThetaBeamBBeam_, cosThetaBeamLBeam_);
    hCosBeamQCosBeamLBeam_->Fill(cosThetaBeamQBeam_, cosThetaBeamLBeam_);
    file_->cd();
  }
  if ( useOffDiagBasis_ ) {
    file_->cd("basisOffDiag");
    treeOffDiag_->Fill();
    hCosOffDiagLOffDiag_->Fill(cosThetaOffDiagLOffDiag_);
    hCosOffDiagBOffDiag_->Fill(cosThetaOffDiagBOffDiag_);
    hCosOffDiagQOffDiag_->Fill(cosThetaOffDiagQOffDiag_);
    hCosOffDiagBCosOffDiagLOffDiag_->Fill(cosThetaOffDiagBOffDiag_, cosThetaOffDiagLOffDiag_);
    hCosOffDiagQCosOffDiagLOffDiag_->Fill(cosThetaOffDiagQOffDiag_, cosThetaOffDiagLOffDiag_);
    file_->cd();
  }

}


/// [FIXME: add further info here.]
void AnalyzeGenEvent::endJob()
{
  file_->cd();

  // extract tt mass split in case of sample splitting by number of events
  splitTtMass_ = splitTtValue_;
  if ( ! splitTtAbsolute_ ) {
    double limit   = std::min(tree_->GetEntriesFast()*splitTtValue_/100., tree_->GetEntriesFast()/1.),
           events  = 0.;
    int    bin     = 0;
    while ( events < limit ) {
      events += hTtMass_->GetBinContent(bin);
      bin++;
    }
    splitTtMass_ = hTtMass_->GetBinLowEdge(bin) + hTtMass_->GetBinWidth(bin)*(events-limit)/hTtMass_->GetBinContent(bin); // just an estimate
  }
  treeMeta_->Fill();

  // fill histos according to (extracted) tt mass split
  tree_->SetBranchAddress("topsZMFMass_", &topsZMFMass_);
  tree_->SetBranchAddress("cosPhiLB_",    &cosPhiLB_);
  tree_->SetBranchAddress("cosPhiLQ_",    &cosPhiLQ_);
  if ( useHelBasis_ ) {
    file_->cd("basisHelicity");
    treeHel_->SetBranchAddress("cosThetaTLHel_", &cosThetaTLHel_);
    treeHel_->SetBranchAddress("cosThetaTBHel_", &cosThetaTBHel_);
    treeHel_->SetBranchAddress("cosThetaTQHel_", &cosThetaTQHel_);
    file_->cd();
  }
  if ( useBeamBasis_ ) {
    file_->cd("basisBeam");
    treeBeam_->SetBranchAddress("cosThetaBeamLBeam_", &cosThetaBeamLBeam_);
    treeBeam_->SetBranchAddress("cosThetaBeamBBeam_", &cosThetaBeamBBeam_);
    treeBeam_->SetBranchAddress("cosThetaBeamQBeam_", &cosThetaBeamQBeam_);
    file_->cd();
  }
  if ( useOffDiagBasis_ ) {
    file_->cd("basisOffDiag");
    treeOffDiag_->SetBranchAddress("cosThetaOffDiagLOffDiag_", &cosThetaOffDiagLOffDiag_);
    treeOffDiag_->SetBranchAddress("cosThetaOffDiagBOffDiag_", &cosThetaOffDiagBOffDiag_);
    treeOffDiag_->SetBranchAddress("cosThetaOffDiagQOffDiag_", &cosThetaOffDiagQOffDiag_);
    file_->cd();
  }
  for ( int entry = 0; entry < tree_->GetEntriesFast(); entry++ ) { // Hopefully the number of entries is the same in all accessed trees...
    tree_->GetEntry(entry);
    if ( topsZMFMass_ < splitTtMass_ ) {
      hAnaCosLB_->Fill(cosPhiLB_);
      hAnaCosLQ_->Fill(cosPhiLQ_);
    } else {
      hRefCosLB_->Fill(cosPhiLB_);
      hRefCosLQ_->Fill(cosPhiLQ_);
    }
    if ( useHelBasis_ ) {
      file_->cd("basisHelicity");
      treeHel_->GetEntry(entry);
      if ( topsZMFMass_ < splitTtMass_ ) {
        hAnaCosTLHel_->Fill(cosThetaTLHel_);
        hAnaCosTBHel_->Fill(cosThetaTBHel_);
        hAnaCosTQHel_->Fill(cosThetaTQHel_);
        hAnaCosTBCosTLHel_->Fill(cosThetaTBHel_, cosThetaTLHel_);
        hAnaCosTQCosTLHel_->Fill(cosThetaTQHel_, cosThetaTLHel_);
      } else {
        hRefCosTLHel_->Fill(cosThetaTLHel_);
        hRefCosTBHel_->Fill(cosThetaTBHel_);
        hRefCosTQHel_->Fill(cosThetaTQHel_);
        hRefCosTBCosTLHel_->Fill(cosThetaTBHel_, cosThetaTLHel_);
        hRefCosTQCosTLHel_->Fill(cosThetaTQHel_, cosThetaTLHel_);
      }
      file_->cd();
    }
    if ( useBeamBasis_ ) {
      file_->cd("basisBeam");
      treeBeam_->GetEntry(entry);
      if ( topsZMFMass_ < splitTtMass_ ) {
        hAnaCosBeamLBeam_->Fill(cosThetaBeamLBeam_);
        hAnaCosBeamBBeam_->Fill(cosThetaBeamBBeam_);
        hAnaCosBeamQBeam_->Fill(cosThetaBeamQBeam_);
        hAnaCosBeamBCosBeamLBeam_->Fill(cosThetaBeamBBeam_, cosThetaBeamLBeam_);
        hAnaCosBeamQCosBeamLBeam_->Fill(cosThetaBeamQBeam_, cosThetaBeamLBeam_);
      } else {
        hRefCosBeamLBeam_->Fill(cosThetaBeamLBeam_);
        hRefCosBeamBBeam_->Fill(cosThetaBeamBBeam_);
        hRefCosBeamQBeam_->Fill(cosThetaBeamQBeam_);
        hRefCosBeamBCosBeamLBeam_->Fill(cosThetaBeamBBeam_, cosThetaBeamLBeam_);
        hRefCosBeamQCosBeamLBeam_->Fill(cosThetaBeamQBeam_, cosThetaBeamLBeam_);
      }
      file_->cd();
    }
    if ( useOffDiagBasis_ ) {
      file_->cd("basisOffDiag");
      treeOffDiag_->GetEntry(entry);
      if ( topsZMFMass_ < splitTtMass_ ) {
        hAnaCosOffDiagLOffDiag_->Fill(cosThetaOffDiagLOffDiag_);
        hAnaCosOffDiagBOffDiag_->Fill(cosThetaOffDiagBOffDiag_);
        hAnaCosOffDiagQOffDiag_->Fill(cosThetaOffDiagQOffDiag_);
        hAnaCosOffDiagBCosOffDiagLOffDiag_->Fill(cosThetaOffDiagBOffDiag_, cosThetaOffDiagLOffDiag_);
        hAnaCosOffDiagQCosOffDiagLOffDiag_->Fill(cosThetaOffDiagQOffDiag_, cosThetaOffDiagLOffDiag_);
      } else {
        hRefCosOffDiagLOffDiag_->Fill(cosThetaOffDiagLOffDiag_);
        hRefCosOffDiagBOffDiag_->Fill(cosThetaOffDiagBOffDiag_);
        hRefCosOffDiagQOffDiag_->Fill(cosThetaOffDiagQOffDiag_);
        hRefCosOffDiagBCosOffDiagLOffDiag_->Fill(cosThetaOffDiagBOffDiag_, cosThetaOffDiagLOffDiag_);
        hRefCosOffDiagQCosOffDiagLOffDiag_->Fill(cosThetaOffDiagQOffDiag_, cosThetaOffDiagLOffDiag_);
      }
      file_->cd();
    }
  }

  // fit histos
  if ( fitHistos_ ) {
    if ( useHelBasis_ ) {
      file_->cd("basisHelicity");
      hCosTLHel_        ->Fit(&fit1L_);
      hCosTBHel_        ->Fit(&fit1B_);
      hCosTQHel_        ->Fit(&fit1Q_);
      hAnaCosTLHel_     ->Fit(&fit1L_);
      hAnaCosTBHel_     ->Fit(&fit1B_);
      hAnaCosTQHel_     ->Fit(&fit1Q_);
      hRefCosTLHel_     ->Fit(&fit1L_);
      hRefCosTBHel_     ->Fit(&fit1B_);
      hRefCosTQHel_     ->Fit(&fit1Q_);
      hCosTBCosTLHel_   ->Fit(&fit2LB_);
      hCosTQCosTLHel_   ->Fit(&fit2LQ_);
      hAnaCosTBCosTLHel_->Fit(&fit2LB_);
      hAnaCosTQCosTLHel_->Fit(&fit2LQ_);
      hRefCosTBCosTLHel_->Fit(&fit2LB_);
      hRefCosTQCosTLHel_->Fit(&fit2LQ_);
      file_->cd();
    }
    if ( useBeamBasis_ ) {
      file_->cd("basisBeam");
      hCosBeamLBeam_           ->Fit(&fit1L_);
      hCosBeamBBeam_           ->Fit(&fit1B_);
      hCosBeamQBeam_           ->Fit(&fit1Q_);
      hAnaCosBeamLBeam_        ->Fit(&fit1L_);
      hAnaCosBeamBBeam_        ->Fit(&fit1B_);
      hAnaCosBeamQBeam_        ->Fit(&fit1Q_);
      hRefCosBeamLBeam_        ->Fit(&fit1L_);
      hRefCosBeamBBeam_        ->Fit(&fit1B_);
      hRefCosBeamQBeam_        ->Fit(&fit1Q_);
      hCosBeamBCosBeamLBeam_   ->Fit(&fit2LB_);
      hCosBeamQCosBeamLBeam_   ->Fit(&fit2LQ_);
      hAnaCosBeamBCosBeamLBeam_->Fit(&fit2LB_);
      hAnaCosBeamQCosBeamLBeam_->Fit(&fit2LQ_);
      hRefCosBeamBCosBeamLBeam_->Fit(&fit2LB_);
      hRefCosBeamQCosBeamLBeam_->Fit(&fit2LQ_);
      file_->cd();
    }
    if ( useOffDiagBasis_ ) {
      file_->cd("basisOffDiag");
      hCosOffDiagLOffDiag_              ->Fit(&fit1L_);
      hCosOffDiagBOffDiag_              ->Fit(&fit1B_);
      hCosOffDiagQOffDiag_              ->Fit(&fit1Q_);
      hAnaCosOffDiagLOffDiag_           ->Fit(&fit1L_);
      hAnaCosOffDiagBOffDiag_           ->Fit(&fit1B_);
      hAnaCosOffDiagQOffDiag_           ->Fit(&fit1Q_);
      hRefCosOffDiagLOffDiag_           ->Fit(&fit1L_);
      hRefCosOffDiagBOffDiag_           ->Fit(&fit1B_);
      hRefCosOffDiagQOffDiag_           ->Fit(&fit1Q_);
      hCosOffDiagBCosOffDiagLOffDiag_   ->Fit(&fit2LB_);
      hCosOffDiagQCosOffDiagLOffDiag_   ->Fit(&fit2LQ_);
      hAnaCosOffDiagBCosOffDiagLOffDiag_->Fit(&fit2LB_);
      hAnaCosOffDiagQCosOffDiagLOffDiag_->Fit(&fit2LQ_);
      hRefCosOffDiagBCosOffDiagLOffDiag_->Fit(&fit2LB_);
      hRefCosOffDiagQCosOffDiagLOffDiag_->Fit(&fit2LQ_);
      file_->cd();
    }
  }

  // write trees & histograms to file
  file_->cd();
  tree_     ->Write();
  treeMeta_ ->Write();
  hTtMass_  ->Write();
  hCosLB_   ->Write();
  hCosLQ_   ->Write();
  hAnaCosLB_->Write();
  hAnaCosLQ_->Write();
  hRefCosLB_->Write();
  hRefCosLQ_->Write();
  if ( useHelBasis_ ) {
    file_->cd("basisHelicity");
    treeHel_          ->Write();
    hCosTLHel_        ->Write();
    hCosTBHel_        ->Write();
    hCosTQHel_        ->Write();
    hAnaCosTLHel_     ->Write();
    hAnaCosTBHel_     ->Write();
    hAnaCosTQHel_     ->Write();
    hRefCosTLHel_     ->Write();
    hRefCosTBHel_     ->Write();
    hRefCosTQHel_     ->Write();
    hCosTBCosTLHel_   ->Write();
    hCosTQCosTLHel_   ->Write();
    hAnaCosTBCosTLHel_->Write();
    hAnaCosTQCosTLHel_->Write();
    hRefCosTBCosTLHel_->Write();
    hRefCosTQCosTLHel_->Write();
    file_->cd();
  }
  if ( useBeamBasis_ ) {
    file_->cd("basisBeam");
    treeBeam_                ->Write();
    hCosBeamLBeam_           ->Write();
    hCosBeamBBeam_           ->Write();
    hCosBeamQBeam_           ->Write();
    hAnaCosBeamLBeam_        ->Write();
    hAnaCosBeamBBeam_        ->Write();
    hAnaCosBeamQBeam_        ->Write();
    hRefCosBeamLBeam_        ->Write();
    hRefCosBeamBBeam_        ->Write();
    hRefCosBeamQBeam_        ->Write();
    hCosBeamBCosBeamLBeam_   ->Write();
    hCosBeamQCosBeamLBeam_   ->Write();
    hAnaCosBeamBCosBeamLBeam_->Write();
    hAnaCosBeamQCosBeamLBeam_->Write();
    hRefCosBeamBCosBeamLBeam_->Write();
    hRefCosBeamQCosBeamLBeam_->Write();
    file_->cd();
  }
  if ( useOffDiagBasis_ ) {
    file_->cd("basisOffDiag");
    treeOffDiag_                      ->Write();
    hCosOffDiagLOffDiag_              ->Write();
    hCosOffDiagBOffDiag_              ->Write();
    hCosOffDiagQOffDiag_              ->Write();
    hAnaCosOffDiagLOffDiag_           ->Write();
    hAnaCosOffDiagBOffDiag_           ->Write();
    hAnaCosOffDiagQOffDiag_           ->Write();
    hRefCosOffDiagLOffDiag_           ->Write();
    hRefCosOffDiagBOffDiag_           ->Write();
    hRefCosOffDiagQOffDiag_           ->Write();
    hCosOffDiagBCosOffDiagLOffDiag_   ->Write();
    hCosOffDiagQCosOffDiagLOffDiag_   ->Write();
    hAnaCosOffDiagBCosOffDiagLOffDiag_->Write();
    hAnaCosOffDiagQCosOffDiagLOffDiag_->Write();
    hRefCosOffDiagBCosOffDiagLOffDiag_->Write();
    hRefCosOffDiagQCosOffDiagLOffDiag_->Write();
    file_->cd();
  }
}
