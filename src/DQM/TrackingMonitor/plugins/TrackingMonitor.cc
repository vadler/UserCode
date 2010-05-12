/*
 *  See header file for a description of this class.
 *
 *  $Date: 2010/05/12 07:14:47 $
 *  $Revision: 1.16 $
 *  \author Suchandra Dutta , Giorgia Mila
 */

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidateCollection.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "MagneticField/Engine/interface/MagneticField.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQM/TrackingMonitor/interface/TrackBuildingAnalyzer.h"
#include "DQM/TrackingMonitor/interface/TrackAnalyzer.h"
#include "DQM/TrackingMonitor/plugins/TrackingMonitor.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "CommonTools/RecoUtils/interface/GenericTriggerEventFlag.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include <string>
#include <iostream> // DEBUG

// TrackingMonitor
// ----------------------------------------------------------------------------------//

TrackingMonitor::TrackingMonitor(const edm::ParameterSet& iConfig)
    : dqmStore_( edm::Service<DQMStore>().operator->() )
    , conf_ ( iConfig )
    , theTrackAnalyzer( new TrackAnalyzer(conf_) )
    , theTrackBuildingAnalyzer( new TrackBuildingAnalyzer(conf_) )
    , NumberOfTracks(NULL)
    , NumberOfMeanRecHitsPerTrack(NULL)
    , NumberOfMeanLayersPerTrack(NULL)
    , NumberOfSeeds(NULL)
    , NumberOfTrackCandidates(NULL)
    , builderName( conf_.getParameter<std::string>("TTRHBuilder"))
    , doTrackerSpecific_( conf_.getParameter<bool>("doTrackerSpecific") )
    , doLumiAnalysis( conf_.getParameter<bool>("doLumiAnalysis"))
    , genTriggerEventFlag_(new GenericTriggerEventFlag(iConfig))
{
  std::cout << "  TrackingMonitor: -> GenericTriggerEventFlag is " << genTriggerEventFlag_->on() << std::endl; // DEBUG
}


TrackingMonitor::~TrackingMonitor()
{
  if (theTrackAnalyzer)          delete theTrackAnalyzer;
  if (theTrackBuildingAnalyzer)  delete theTrackBuildingAnalyzer;
  if (genTriggerEventFlag_)      delete genTriggerEventFlag_;
}


void TrackingMonitor::beginJob(void)
{

    // parameters from the configuration
    std::string Quality      = conf_.getParameter<std::string>("Quality");
    std::string AlgoName     = conf_.getParameter<std::string>("AlgoName");
    std::string MEFolderName = conf_.getParameter<std::string>("FolderName");

    // test for the Quality veriable validity
    if( Quality != "")
    {
        if( Quality != "highPurity" && Quality != "tight" && Quality != "loose")
        {
            edm::LogWarning("TrackingMonitor")  << "Qualty Name is invalid, using no quality criterea by default";
            Quality = "";
        }
    }

    // use the AlgoName and Quality Name
    std::string CatagoryName = Quality != "" ? AlgoName + "_" + Quality : AlgoName;

    // get binning from the configuration
    int    TKNoBin     = conf_.getParameter<int>(   "TkSizeBin");
    double TKNoMin     = conf_.getParameter<double>("TkSizeMin");
    double TKNoMax     = conf_.getParameter<double>("TkSizeMax");

    int    TCNoBin     = conf_.getParameter<int>(   "TCSizeBin");
    double TCNoMin     = conf_.getParameter<double>("TCSizeMin");
    double TCNoMax     = conf_.getParameter<double>("TCSizeMax");

    int    TKNoSeedBin = conf_.getParameter<int>(   "TkSeedSizeBin");
    double TKNoSeedMin = conf_.getParameter<double>("TkSeedSizeMin");
    double TKNoSeedMax = conf_.getParameter<double>("TkSeedSizeMax");

    int    MeanHitBin  = conf_.getParameter<int>(   "MeanHitBin");
    double MeanHitMin  = conf_.getParameter<double>("MeanHitMin");
    double MeanHitMax  = conf_.getParameter<double>("MeanHitMax");

    int    MeanLayBin  = conf_.getParameter<int>(   "MeanLayBin");
    double MeanLayMin  = conf_.getParameter<double>("MeanLayMin");
    double MeanLayMax  = conf_.getParameter<double>("MeanLayMax");

    std::string StateName = conf_.getParameter<std::string>("MeasurementState");
    if
    (
        StateName != "OuterSurface" &&
        StateName != "InnerSurface" &&
        StateName != "ImpactPoint"  &&
        StateName != "default"      &&
        StateName != "All"
    )
    {
        // print warning
        edm::LogWarning("TrackingMonitor")  << "State Name is invalid, using 'ImpactPoint' by default";
    }

    dqmStore_->setCurrentFolder(MEFolderName);

    // book the General Property histograms
    // ---------------------------------------------------------------------------------//
    dqmStore_->setCurrentFolder(MEFolderName+"/GeneralProperties");

    histname = "NumberOfTracks_" + CatagoryName;
    NumberOfTracks = dqmStore_->book1D(histname, histname, TKNoBin, TKNoMin, TKNoMax);
    NumberOfTracks->setAxisTitle("Number of Tracks per Event", 1);
    NumberOfTracks->setAxisTitle("Number of Events", 2);

    histname = "NumberOfMeanRecHitsPerTrack_" + CatagoryName;
    NumberOfMeanRecHitsPerTrack = dqmStore_->book1D(histname, histname, MeanHitBin, MeanHitMin, MeanHitMax);
    NumberOfMeanRecHitsPerTrack->setAxisTitle("Mean number of RecHits per Track", 1);
    NumberOfMeanRecHitsPerTrack->setAxisTitle("Entries", 2);

    histname = "NumberOfMeanLayersPerTrack_" + CatagoryName;
    NumberOfMeanLayersPerTrack = dqmStore_->book1D(histname, histname, MeanLayBin, MeanLayMin, MeanLayMax);
    NumberOfMeanLayersPerTrack->setAxisTitle("Mean number of Layers per Track", 1);
    NumberOfMeanLayersPerTrack->setAxisTitle("Entries", 2);

    theTrackAnalyzer->beginJob(dqmStore_);

    // book the Seed Property histograms
    // ---------------------------------------------------------------------------------//
    if (conf_.getParameter<bool>("doSeedParameterHistos"))
    {
        dqmStore_->setCurrentFolder(MEFolderName+"/TrackBuilding");

        histname = "NumberOfSeeds_" + CatagoryName;
        NumberOfSeeds = dqmStore_->book1D(histname, histname, TKNoSeedBin, TKNoSeedMin, TKNoSeedMax);
        NumberOfSeeds->setAxisTitle("Number of Seeds per Event", 1);
        NumberOfSeeds->setAxisTitle("Number of Events", 2);

        histname = "NumberOfTrackCandidates_" + CatagoryName;
        NumberOfTrackCandidates = dqmStore_->book1D(histname, histname, TCNoBin, TCNoMin, TCNoMax);
        NumberOfTrackCandidates->setAxisTitle("Number of Track Candidates per Event", 1);
        NumberOfTrackCandidates->setAxisTitle("Number of Event", 2);

        theTrackBuildingAnalyzer->beginJob(dqmStore_);
    }
    if (doLumiAnalysis) {
      if (NumberOfTracks) NumberOfTracks->setLumiFlag();
      theTrackAnalyzer->setLumiFlag();
    }
    if (doTrackerSpecific_) {
      int    NClusPxBin  = conf_.getParameter<int>(   "NClusPxBin");
      double NClusPxMin  = conf_.getParameter<double>("NClusPxMin");
      double NClusPxMax = conf_.getParameter<double>("NClusPxMax");


      int    NClusStrBin = conf_.getParameter<int>(   "NClusStrBin");
      double NClusStrMin = conf_.getParameter<double>("NClusStrMin");
      double NClusStrMax = conf_.getParameter<double>("NClusStrMax");

      int    NClus2DStrBin = conf_.getParameter<int>(   "NClus2DStrBin");
      double NClus2DStrMin = conf_.getParameter<double>("NClus2DStrMin");
      double NClus2DStrMax = conf_.getParameter<double>("NClus2DStrMax");
      int    NClus2DPxBin  = conf_.getParameter<int>(   "NClus2DPxBin");
      double NClus2DPxMin  = conf_.getParameter<double>("NClus2DPxMin");
      double NClus2DPxMax  = conf_.getParameter<double>("NClus2DPxMax");

      int    NClus2DTotBin = conf_.getParameter<int>(   "NClus2DTotBin");
      double NClus2DTotMin = conf_.getParameter<double>("NClus2DTotMin");
      double NClus2DTotMax = conf_.getParameter<double>("NClus2DTotMax");
      int    NTrk2DBin     = conf_.getParameter<int>(   "NTrk2DBin");
      double NTrk2DMin     = conf_.getParameter<double>("NTrk2DMin");
      double NTrk2DMax     = conf_.getParameter<double>("NTrk2DMax");

      dqmStore_->setCurrentFolder(MEFolderName+"/HitProperties");
      histname = "NumberOfClustersInPixel_" + CatagoryName;
      NumberOfPixelClus = dqmStore_->book1D(histname, histname, NClusPxBin, NClusPxMin, NClusPxMax);
      NumberOfPixelClus->setAxisTitle("# of Clusters in Pixel", 1);
      NumberOfPixelClus->setAxisTitle("Number of Events", 2);

      histname = "NumberOfClustersInStrip_" + CatagoryName;
      NumberOfStripClus = dqmStore_->book1D(histname, histname, NClusStrBin, NClusStrMin, NClusStrMax);
      NumberOfStripClus->setAxisTitle("# of Clusters in Strip Detectors", 1);
      NumberOfStripClus->setAxisTitle("Number of Events", 2);

      histname = "PixelClustersVsStripClusters_" + CatagoryName;
      NumberOfStripVsStripClus = dqmStore_->book2D(histname, histname, NClus2DStrBin, NClus2DStrMin, NClus2DStrMax,
                                                                NClus2DPxBin, NClus2DPxMin,  NClus2DPxMax);
      NumberOfStripVsStripClus->setAxisTitle("# of Clusters in Pixel Detectors", 1);
      NumberOfStripVsStripClus->setAxisTitle("# of Clusters in Strip Detectors", 2);

      histname = "RatioOfPixelAndStripClusters_" + CatagoryName;
      RatioOfPixelAndStripClus = dqmStore_->book1D(histname, histname, 100, 0.0, 1.6);
      RatioOfPixelAndStripClus->setAxisTitle("ArcTan(PixelCluster/StripClusters)", 1);
      RatioOfPixelAndStripClus->setAxisTitle("Number of Events", 2);

      histname = "TracksVsClusters_" + CatagoryName;
      NumberOfTrkVsClus = dqmStore_->book2D(histname,histname,NTrk2DBin,NTrk2DMin,NTrk2DMax,
                                                       NClus2DTotBin,NClus2DTotMin,NClus2DTotMax);
      NumberOfTrkVsClus->setAxisTitle("Number of Tracks", 1);
      NumberOfTrkVsClus->setAxisTitle("# of Clusters in (Pixel+Strip) Detectors", 2);

    }


}

// -- BeginRun
//---------------------------------------------------------------------------------//
void TrackingMonitor::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
  // Initialize the GenericTriggerEventFlag
  if ( genTriggerEventFlag_->on() ) genTriggerEventFlag_->initRun( iRun, iSetup );
}

// - BeginLumi
// ---------------------------------------------------------------------------------//
void TrackingMonitor::beginLuminosityBlock(const edm::LuminosityBlock& lumi, const edm::EventSetup&  eSetup) {
  if (doLumiAnalysis) {
    dqmStore_->softReset(NumberOfTracks);
    theTrackAnalyzer->doSoftReset(dqmStore_);
  }
}

// -- Analyse
// ---------------------------------------------------------------------------------//
void TrackingMonitor::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    // Filter out events if Trigger Filtering is requested
// DEBUG     if (genTriggerEventFlag_->on()&& ! genTriggerEventFlag_->accept( iEvent, iSetup) ) return;
  static unsigned count( 0 ); // DEBUG
  std::cout << "* TrackingMonitor *" << std::endl; // DEBUG
  bool decision( true ); // DEBUG
  if ( genTriggerEventFlag_->on() ) decision = genTriggerEventFlag_->accept( iEvent, iSetup ); // DEBUG
  std::cout << "  TrackingMonitor: -> " << decision << " (count: "; // DEBUG
  if ( ! decision ) { // DEBUG
    std::cout << count << ")" << std::endl; // DEBUG
    return; // DEBUG
  } // DEBUG
  std::cout << ++count << ")" << std::endl; // DEBUG

    // input tags for collections from the configuration
    edm::InputTag trackProducer  = conf_.getParameter<edm::InputTag>("TrackProducer");
    edm::InputTag seedProducer   = conf_.getParameter<edm::InputTag>("SeedProducer");
    edm::InputTag tcProducer     = conf_.getParameter<edm::InputTag>("TCProducer");
    edm::InputTag bsSrc          = conf_.getParameter<edm::InputTag>("beamSpot");
    std::string Quality     = conf_.getParameter<std::string>("Quality");
    std::string Algo        = conf_.getParameter<std::string>("AlgoName");

    //  Analyse the tracks
    //  if the collection is empty, do not fill anything
    // ---------------------------------------------------------------------------------//

    // get the track collection
    edm::Handle<reco::TrackCollection> trackHandle;
    iEvent.getByLabel(trackProducer, trackHandle);

    if (trackHandle.isValid())
    {

       reco::TrackCollection trackCollection = *trackHandle;
        // calculate the mean # rechits and layers
        int totalNumTracks = 0, totalRecHits = 0, totalLayers = 0;

        for (reco::TrackCollection::const_iterator track = trackCollection.begin(); track!=trackCollection.end(); ++track)
        {

            if( Quality == "highPurity")
            {
                if( !track->quality(reco::TrackBase::highPurity) ) continue;
            }
            else if( Quality == "tight")
            {
                if( !track->quality(reco::TrackBase::tight) ) continue;
            }
            else if( Quality == "loose")
            {
                if( !track->quality(reco::TrackBase::loose) ) continue;
            }

            totalNumTracks++;
            totalRecHits    += track->found();
            totalLayers     += track->hitPattern().trackerLayersWithMeasurement();

            // do analysis per track
            theTrackAnalyzer->analyze(iEvent, iSetup, *track);
        }

        NumberOfTracks->Fill(totalNumTracks);

        if( totalNumTracks > 0 )
        {
            double meanRecHits = static_cast<double>(totalRecHits) / static_cast<double>(totalNumTracks);
            double meanLayers  = static_cast<double>(totalLayers)  / static_cast<double>(totalNumTracks);
            NumberOfMeanRecHitsPerTrack->Fill(meanRecHits);
            NumberOfMeanLayersPerTrack->Fill(meanLayers);
        }


	//  Analyse the Track Building variables
	//  if the collection is empty, do not fill anything
	// ---------------------------------------------------------------------------------//

	if (conf_.getParameter<bool>("doSeedParameterHistos"))
	  {

	    // magnetic field
	    edm::ESHandle<MagneticField> theMF;
	    iSetup.get<IdealMagneticFieldRecord>().get(theMF);

	    // get the beam spot
	    edm::Handle<reco::BeamSpot> recoBeamSpotHandle;
	    iEvent.getByLabel(bsSrc,recoBeamSpotHandle);
	    const reco::BeamSpot& bs = *recoBeamSpotHandle;

	    // get the candidate collection
	    edm::Handle<TrackCandidateCollection> theTCHandle;
	    iEvent.getByLabel(tcProducer, theTCHandle );
	    const TrackCandidateCollection& theTCCollection = *theTCHandle;

	    // fill the TrackCandidate info
	    if (theTCHandle.isValid())
	      {
		NumberOfTrackCandidates->Fill(theTCCollection.size());
		iSetup.get<TransientRecHitRecord>().get(builderName,theTTRHBuilder);
		for( TrackCandidateCollection::const_iterator cand = theTCCollection.begin(); cand != theTCCollection.end(); ++cand)
		  {
		    theTrackBuildingAnalyzer->analyze(iEvent, iSetup, *cand, bs, theMF, theTTRHBuilder);
		  }
	      }
	    else
	      {
		edm::LogWarning("TrackingMonitor") << "No Track Candidates in the event.  Not filling associated histograms";
	      }

	    // get the seed collection
	    edm::Handle<edm::View<TrajectorySeed> > seedHandle;
	    iEvent.getByLabel(seedProducer, seedHandle);
	    const edm::View<TrajectorySeed>& seedCollection = *seedHandle;

	    // fill the seed info
	    if (seedHandle.isValid())
	      {
		NumberOfSeeds->Fill(seedCollection.size());

		iSetup.get<TransientRecHitRecord>().get(builderName,theTTRHBuilder);
		for(size_t i=0; i < seedHandle->size(); ++i)
		  {
		    edm::RefToBase<TrajectorySeed> seed(seedHandle, i);
		    theTrackBuildingAnalyzer->analyze(iEvent, iSetup, *seed, bs, theMF, theTTRHBuilder);
		  }
	      }
	    else
	      {
		edm::LogWarning("TrackingMonitor") << "No Trajectory seeds in the event.  Not filling associated histograms";
	      }
	  }
	if ( doTrackerSpecific_)
          {
	    edm::Handle< edmNew::DetSetVector<SiStripCluster> > strip_clusters;
	    iEvent.getByLabel("siStripClusters", strip_clusters);
	    edm::Handle< edmNew::DetSetVector<SiPixelCluster> > pixel_clusters;
	    iEvent.getByLabel("siPixelClusters", pixel_clusters);
            if (strip_clusters.isValid() && pixel_clusters.isValid())
              {
                unsigned int ncluster_pix   = (*pixel_clusters).size(); // size_type will be better
                unsigned int ncluster_strip = (*strip_clusters).size(); // size_type will be better
                double ratio = 0.0;
                if ( ncluster_pix > 0) ratio = atan(ncluster_pix*1.0/ncluster_strip);

                NumberOfStripClus->Fill(ncluster_strip);
		NumberOfPixelClus->Fill(ncluster_pix);
                NumberOfStripVsStripClus->Fill(ncluster_strip,ncluster_pix);
		RatioOfPixelAndStripClus->Fill(ratio);
		NumberOfTrkVsClus->Fill(totalNumTracks, ncluster_strip+ncluster_pix);
              }
          }

    }
    else
    {
        return;
    }
}
void TrackingMonitor::endRun(const edm::Run&, const edm::EventSetup&)
{
  if (doLumiAnalysis) {
    dqmStore_->disableSoftReset(NumberOfTracks);
    theTrackAnalyzer->undoSoftReset(dqmStore_);
  }

}

void TrackingMonitor::endJob(void)
{
    bool outputMEsInRootFile   = conf_.getParameter<bool>("OutputMEsInRootFile");
    std::string outputFileName = conf_.getParameter<std::string>("OutputFileName");
    if(outputMEsInRootFile)
    {
        dqmStore_->showDirStructure();
        dqmStore_->save(outputFileName);
    }
}

DEFINE_FWK_MODULE(TrackingMonitor);
