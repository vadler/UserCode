#ifndef SiStripMonitorTrack_H
#define SiStripMonitorTrack_H

// system include files
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/CommonDetUnit/interface/GeomDetType.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetNew.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"

#include "AnalysisDataFormats/SiStripClusterInfo/interface/SiStripClusterInfo.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "CalibFormats/SiStripObjects/interface/SiStripDetCabling.h"

#include "DQM/SiStripCommon/interface/SiStripFolderOrganizer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"

#include "CommonTools/RecoUtils/interface/GenericTriggerEventFlag.h"

//******** Single include for the TkMap *************
#include "DQM/SiStripCommon/interface/TkHistoMap.h"
//***************************************************

class SiStripDCSStatus;
//
// class declaration
//

class SiStripMonitorTrack : public edm::EDAnalyzer {
public:
  typedef TransientTrackingRecHit::ConstRecHitPointer ConstRecHitPointer;
  enum RecHitType { Single=0, Matched=1, Projected=2, Null=3};
  explicit SiStripMonitorTrack(const edm::ParameterSet&);
  ~SiStripMonitorTrack();
  virtual void beginRun(const edm::Run& run, const edm::EventSetup& c);
  virtual void endJob(void);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);

private:
  //booking
  void book();
  void bookModMEs(std::string, uint32_t);
  void bookTrendMEs(std::string, int32_t,uint32_t,std::string flag);
  void bookSubDetMEs(std::string& name,std::string& flag);
  MonitorElement * bookME1D(const char*, const char*);
  MonitorElement * bookME2D(const char*, const char*);
  MonitorElement * bookME3D(const char*, const char*);
  MonitorElement * bookMEProfile(const char*, const char*);
  MonitorElement * bookMETrend(const char*, const char*);
  // internal evaluation of monitorables
  void AllClusters(const edm::Event& ev, const edm::EventSetup& es);
  void trackStudy(const edm::Event& ev, const edm::EventSetup& es);
  //  LocalPoint project(const GeomDet *det,const GeomDet* projdet,LocalPoint position,LocalVector trackdirection)const;
  bool clusterInfos(SiStripClusterInfo* cluster, const uint32_t& detid,std::string flag, LocalVector LV);
  template <class T> void RecHitInfo(const T* tkrecHit, LocalVector LV,reco::TrackRef track_ref, const edm::EventSetup&);

  // fill monitorables
  void fillModMEs(SiStripClusterInfo*,std::string,float);
  void fillMEs(SiStripClusterInfo*,uint32_t detid,float,std::string);
  inline void fillME(MonitorElement* ME,float value1){if (ME!=0)ME->Fill(value1);}
  inline void fillME(MonitorElement* ME,float value1,float value2){if (ME!=0)ME->Fill(value1,value2);}
  inline void fillME(MonitorElement* ME,float value1,float value2,float value3){if (ME!=0)ME->Fill(value1,value2,value3);}
  inline void fillME(MonitorElement* ME,float value1,float value2,float value3,float value4){if (ME!=0)ME->Fill(value1,value2,value3,value4);}

  // ----------member data ---------------------------

private:
  DQMStore * dbe;
  edm::ParameterSet conf_;
  std::string histname;
  LocalVector LV;
  float iOrbitSec;

  //******* TkHistoMaps
  TkHistoMap *tkhisto_StoNCorrOnTrack, *tkhisto_NumOnTrack, *tkhisto_NumOffTrack;
  //******** TkHistoMaps

  struct ModMEs{
    ModMEs():
      ClusterStoN(0),
	 ClusterStoNCorr(0),
	 ClusterCharge(0),
	 ClusterChargeCorr(0),
	 ClusterWidth(0),
	 ClusterPos(0),
	 ClusterPGV(0){};
    MonitorElement* ClusterStoN;
    MonitorElement* ClusterStoNCorr;
    MonitorElement* ClusterCharge;
    MonitorElement* ClusterChargeCorr;
    MonitorElement* ClusterWidth;
    MonitorElement* ClusterPos;
    MonitorElement* ClusterPGV;
  };

  struct LayerMEs{
    LayerMEs():
      nClusters(0),
      nClustersTrend(0),
      ClusterStoN(0),
      ClusterStoNCorr(0),
      ClusterStoNTrend(0),
      ClusterStoNCorrTrend(0),
      ClusterCharge(0),
      ClusterChargeCorr(0),
      ClusterChargeTrend(0),
      ClusterChargeCorrTrend(0),
      ClusterNoise(0),
      ClusterNoiseTrend(0),
      ClusterWidth(0),
      ClusterWidthTrend(0),
      ClusterPos(0),
      ClusterPGV(0){};
    MonitorElement* nClusters;
    MonitorElement* nClustersTrend;
    MonitorElement* ClusterStoN;
    MonitorElement* ClusterStoNCorr;
    MonitorElement* ClusterStoNTrend;
    MonitorElement* ClusterStoNCorrTrend;
    MonitorElement* ClusterCharge;
    MonitorElement* ClusterChargeCorr;
    MonitorElement* ClusterChargeTrend;
    MonitorElement* ClusterChargeCorrTrend;
    MonitorElement* ClusterNoise;
    MonitorElement* ClusterNoiseTrend;
    MonitorElement* ClusterWidth;
    MonitorElement* ClusterWidthTrend;
    MonitorElement* ClusterPos;
    MonitorElement* ClusterPGV;
  };

  std::map<std::string, ModMEs> ModMEsMap;
  std::map<std::string, LayerMEs> LayerMEsMap;
  std::map<std::string, MonitorElement*> MEMap;

  edm::ESHandle<TrackerGeometry> tkgeom;
  edm::ESHandle<SiStripDetCabling> SiStripDetCabling_;

  edm::ParameterSet Parameters;
  edm::InputTag Cluster_src_;

  bool Mod_On_;
  bool Trend_On_;
  bool OffHisto_On_;
  bool HistoFlag_On_;
  bool ring_flag;
  bool TkHistoMap_On_;

  std::string TrackProducer_;
  std::string TrackLabel_;

  int off_Flag;
  std::vector<uint32_t> ModulesToBeExcluded_;
  std::vector<const SiStripCluster*> vPSiStripCluster;
  std::map<std::pair<std::string,int32_t>,bool> DetectedLayers;
  SiStripFolderOrganizer folder_organizer;
  bool tracksCollection_in_EventTree;
  bool trackAssociatorCollection_in_EventTree;
  bool flag_ring;
  int runNb, eventNb;
  int firstEvent;
  int countOn, countOff, countAll, NClus[4][3];

  bool   applyClusterQuality_;
  double sToNLowerLimit_;
  double sToNUpperLimit_;
  double widthLowerLimit_;
  double widthUpperLimit_;

  SiStripDCSStatus* dcsStatus_;

  GenericTriggerEventFlag * eventFlag;
};
#endif
