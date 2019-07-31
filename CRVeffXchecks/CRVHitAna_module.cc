#include <iostream>
#include <string>
#include <limits>
#include <map>
#include <algorithm>
#include "TTree.h"
#include "TNtuple.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CosmicRayShieldGeom/inc/CosmicRayShield.hh"
#include "DataProducts/inc/CRSScintillatorBarIndex.hh"
#include "ConditionsService/inc/AcceleratorParams.hh"
#include "ConditionsService/inc/ConditionsHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/GeometryService.hh"
#include "HepPID/ParticleName.hh"
#include "MCDataProducts/inc/GenParticle.hh"
#include "MCDataProducts/inc/GenParticleCollection.hh"
#include "MCDataProducts/inc/PhysicalVolumeInfoMultiCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "MCDataProducts/inc/StepInstanceName.hh"
#include "MCDataProducts/inc/MCTrajectoryCollection.hh"
#include "Mu2eUtilities/inc/PhysicalVolumeMultiHelper.hh"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "fhiclcpp/ParameterSet.h"
#include "RecoDataProducts/inc/CrvCoincidenceClusterSummaryCollection.hh"

namespace mu2e
{

  const double nan = std::numeric_limits<double>::quiet_NaN();

  //Structure to contain hit information for particles for filling tree
  struct CRVHit{
    double stepTime;
    double X, Y, Z;
    double InitX, InitY, InitZ;
    double InitPx, InitPy, InitPz;
    double parentInitX, parentInitY, parentInitZ;
    double stepEDep, totalEDep, nonIonEDep, visEn;
    double momentum, px, py, pz;
    double layerLengthX, layerLengthY, layerLengthZ;
    int    EventID, sipm, pe;
    int    section, numModulesInSection, module, sector;
    int    layer, counter, barIndex;
    int    PDGid, parentPDGid;
    char   productionVolumeName[100];
    char   productionProcessName[100];
    int hasCoincidence;
    double coinX, coinY, coinZ;
  }; // struct CRVHit

  class CRVHitAna : public art::EDAnalyzer
  {
  public:
    explicit CRVHitAna(fhicl::ParameterSet const &pset);
    virtual ~CRVHitAna() { }
    virtual void beginJob();
    void endJob();
    void analyze(const art::Event& e);
    int getCrvSection(int shieldID);

  private:
    //std::string _g4ModuleLabel;
    std::string _volumeModuleLabel;
    std::string _processName;
    std::string _crvCoincidences;
    double  _evEDep;
    int    _verboseLevel;

    TTree*   _crvHits; //Tree to be filled with hit information
    CRVHit   hit_; //Struct to act as a buffer for filling tree with hit information
  };

  CRVHitAna::CRVHitAna(fhicl::ParameterSet const &pset) :
    art::EDAnalyzer(pset),
    _volumeModuleLabel(pset.get<std::string>("volumeModuleLabel")),
    _processName(pset.get<std::string>("processName")),
    _crvCoincidences(pset.get<std::string>("crvCoincidences")),
    _verboseLevel(pset.get<int>("verboseLevel"))
  {
  }

  void CRVHitAna::beginJob()
  {
    CRVHit &h = hit_;
    art::ServiceHandle<art::TFileService> tfs;
    _crvHits = tfs->make<TTree>("crvHits", "CRV Hit Tree"); //Make tree
    _crvHits->Branch("stepTime", &h.stepTime, "stepTime/D");
    _crvHits->Branch("X", &h.X, "X/D");
    _crvHits->Branch("Y", &h.Y, "Y/D");
    _crvHits->Branch("Z", &h.Z, "Z/D");
    _crvHits->Branch("InitX", &h.InitX, "InitX/D");
    _crvHits->Branch("InitY", &h.InitY, "InitY/D");
    _crvHits->Branch("InitZ", &h.InitZ, "InitZ/D");
    _crvHits->Branch("InitPx", &h.InitPx, "InitPx/D");
    _crvHits->Branch("InitPy", &h.InitPy, "InitPy/D");
    _crvHits->Branch("InitPz", &h.InitPz, "InitPz/D");
    _crvHits->Branch("parentInitX", &h.parentInitX, "parentInitX/D");
    _crvHits->Branch("parentInitY", &h.parentInitY, "parentInitY/D");
    _crvHits->Branch("parentInitZ", &h.parentInitZ, "parentInitZ/D");
    _crvHits->Branch("stepEDep", &h.stepEDep, "stepEDep/D");
    _crvHits->Branch("totalEDep", &h.totalEDep, "totalEDep/D");
    _crvHits->Branch("nonIonEDep", &h.nonIonEDep, "nonIonEDep/D");
    _crvHits->Branch("visEn", &h.visEn, "visEn/D");
    _crvHits->Branch("momentum", &h.momentum, "momentum/D");
    _crvHits->Branch("px", &h.px, "px/D");
    _crvHits->Branch("py", &h.py, "py/D");
    _crvHits->Branch("pz", &h.pz, "pz/D"); 
    _crvHits->Branch("layerLengthX", &h.layerLengthX, "layerLengthX/D");
    _crvHits->Branch("layerLengthY", &h.layerLengthY, "layerLengthY/D");
    _crvHits->Branch("layerLengthZ", &h.layerLengthZ, "layerLengthZ/D");
    _crvHits->Branch("EventID", &h.EventID, "EventID/I");
    _crvHits->Branch("sipm", &h.sipm, "sipm/I");
    _crvHits->Branch("pe", &h.pe, "pe/I");
    _crvHits->Branch("section", &h.section, "section/I");
    _crvHits->Branch("sector", &h.sector, "sector/I");
    _crvHits->Branch("numModulesInSection", &h.numModulesInSection, "numModulesInSection/I");
    _crvHits->Branch("module", &h.module, "module/I");
    _crvHits->Branch("layer", &h.layer, "layer/I");
    _crvHits->Branch("counter", &h.counter, "counter/I");
    _crvHits->Branch("barIndex", &h.barIndex, "barIndex/I");
    _crvHits->Branch("PDGid", &h.PDGid, "PDGid/I");
    _crvHits->Branch("parentPDGid", &h.parentPDGid, "parentPDGid/I");
    _crvHits->Branch("productionVolumeName", &h.productionVolumeName, "productionVolumeName[100]/C");
    _crvHits->Branch("productionProcessName", &h.productionProcessName, "productionProcessName[100]/C");
    _crvHits->Branch("hasCoincidence", &h.hasCoincidence, "hasCoincidence/I");
    _crvHits->Branch("coinX", &h.coinX, "coinX/D");
    _crvHits->Branch("coinY", &h.coinY, "coinY/D");
    _crvHits->Branch("coinZ", &h.coinZ, "coinZ/D");
  }

  void CRVHitAna::endJob()
  {
  }

  void CRVHitAna::analyze(const art::Event& event)
  {
    
    //This selector will select only data products with the given instance name
    std::unique_ptr<art::Selector> selector;
    typedef std::vector< art::Handle<StepPointMCCollection> > HandleVector;
    HandleVector stepsHandles;

    //Select events - select events with a process name if specified (typically for cosmic files)
    //the ProcessNameSelector allows "*" and "" if one wants to get anything with instanceName = CRV
    if (_processName != "" && _processName != "*")
      selector = std::unique_ptr<art::Selector>(new art::Selector(art::ProductInstanceNameSelector("CRV") && art::ProcessNameSelector(_processName)));
    else
      selector = std::unique_ptr<art::Selector>(new art::Selector(art::ProductInstanceNameSelector("CRV")));
    
    event.getMany(*selector, stepsHandles); //Get all of the CRV StepPointMC collections from the event
    art::Handle<mu2e::SimParticleCollection> simParticleCollection;
    event.getByLabel("g4filter", "s0", simParticleCollection);
    art::Handle<mu2e::MCTrajectoryCollection> mcTrajectories;
    event.getByLabel("g4filter", "", mcTrajectories);    

    //If stepsHandles was empty, then there were no StepPointMCs relating to the CRV
    if (stepsHandles.empty())      
      throw cet::exception("SIM") << "mu2e: CRVHitAnalysis-CRVHitAna: No StepPointMC collections found for CRV" << std::endl;
      
    //Volume information
    art::Handle<mu2e::PhysicalVolumeInfoMultiCollection> physicalVolumesMulti;
    bool hasPhysicalVolumes = event.getSubRun().getByLabel(_volumeModuleLabel, physicalVolumesMulti);
    //Geometry handle
    GeomHandle<CosmicRayShield> CRS;

    if(event.id().event()%1000==0)
      std::cout << "----- Processing event:" << event.id().event() << " -----" << std::endl;

    // Check if there is a concidence

    hit_.hasCoincidence = 0;
    art::Handle<CrvCoincidenceClusterSummaryCollection> crvCoincidenceClusterSummaryCollection;
    if(!event.getByLabel(_crvCoincidences,"",crvCoincidenceClusterSummaryCollection)) return;
    if(crvCoincidenceClusterSummaryCollection.product()!=NULL){
      hit_.hasCoincidence = crvCoincidenceClusterSummaryCollection->size();
      std::vector<CrvCoincidenceClusterSummary>::const_iterator iter;
      for(iter=crvCoincidenceClusterSummaryCollection->begin(); iter!=crvCoincidenceClusterSummaryCollection->end(); iter++){
	const CrvCoincidenceClusterSummary &cluster = *iter;
	hit_.coinX = cluster.GetAvgCounterPos().x();
	hit_.coinY = cluster.GetAvgCounterPos().y();
	hit_.coinZ = cluster.GetAvgCounterPos().z();
      }
    }

    //-------------------------------------------------------------------------------------------------------
    //		Loop StepPointMCs and fill hit tree
    //-------------------------------------------------------------------------------------------------------


    int lastTrkIndx = -1, lastLayer = -1, lastEvNum = -1;
    //Loop over StepPointMC collections
    for (HandleVector::const_iterator ispmcc = stepsHandles.begin(); ispmcc != stepsHandles.end(); ++ispmcc){
      art::Handle<StepPointMCCollection> const& handle(*ispmcc);
      StepPointMCCollection const& steps(*handle);
      
      //Loop over the StepPointMCs in this collection
      for (size_t ispmc = 0; ispmc < steps.size(); ++ispmc){
	
	_evEDep += (double) steps[ispmc].totalEDep();
	const CRSScintillatorBarId &CRVCounterId = CRS->getBar(steps[ispmc].barIndex()).id();
	const CRSScintillatorLayer &layer = CRS->getLayer(CRVCounterId.getLayerId());

	int trkIndx = steps[ispmc].simParticle()->id().asInt();
	int barIndex = steps[ispmc].barIndex().asInt();
	int Layer = CRVCounterId.getLayerNumber();
	int evNum = event.id().event();

	//if it is the same particle in the same crv layer, skip adding it to the tree 
	//MODIFIED: removed bar index check
	if ((trkIndx == lastTrkIndx) && (Layer == lastLayer) && (evNum == lastEvNum))
	  continue;
	
	int PDGid = steps[ispmc].simParticle()->pdgId();
	std::string particleName = HepPID::particleName(PDGid);
	//Skip soft photons
	if (PDGid == 22 && steps[ispmc].momentum().mag() < 0.001) continue;
	// SKIP ALL PARTICLES EXCEPT MUONS
	if (abs(PDGid) != 13) continue;
	
	int Section = getCrvSection(CRVCounterId.getShieldNumber());
	int Sector  = CRVCounterId.getShieldNumber();
	int Module  = CRVCounterId.getModuleNumber();
	int Counter = CRVCounterId.getBarNumber();
	
	CLHEP::Hep3Vector CRVHitPosition = steps[ispmc].position();
	double stepTime = (double) steps[ispmc].time();	  
	double stepEDep = steps[ispmc].totalEDep();
	double totEDep = 0;
	double nonIonEDep = 0;
	for (size_t ispmc_subStep = ispmc; ispmc_subStep < steps.size(); ++ispmc_subStep){	    
	  //If the other step does not have an energy deposition, is not in the same bar, or is outside of the time window, then skip
	  if (steps[ispmc_subStep].totalEDep() < 0) continue;
	  if (CRS->getBar(steps[ispmc_subStep].barIndex()).id().getLayerNumber() != Layer) continue;
	  if (getCrvSection(CRS->getBar(steps[ispmc_subStep].barIndex()).id().getShieldNumber())) continue;
	  totEDep += (double) steps[ispmc_subStep].totalEDep();
	  nonIonEDep += (double) steps[ispmc_subStep].nonIonizingEDep();
	}

	//this sim particle may have been create in the previous simulation stage
	//therefore, a particle which has the creation code "primary", may not be a primary
	//but a continuation of a particle from a previous stage.
	//this particle from the previous stage is this particle's parent
	art::Ptr<mu2e::SimParticle> simparticle = steps[ispmc].simParticle();
	if (simparticle->creationCode() == ProcessCode::mu2ePrimary &&
	    simparticle->hasParent()) simparticle = simparticle->parent();
	
	ProcessCode productionProcess = simparticle->creationCode();
	std::string productionProcessName = productionProcess.name();
	std::string productionVolumeName = "unknown volume";
	
	if(hasPhysicalVolumes)
	  std::cout << "hasPhysicalVolumes:" << hasPhysicalVolumes << std::endl;
	// if(physicalVolumesMulti.product() != NULL)
	//   std::cout << "physicalVolumeMulti:" << physicalVolumesMulti.product() << std::endl;
	if (hasPhysicalVolumes && physicalVolumesMulti.product() != NULL)
	  {
	    mu2e::PhysicalVolumeMultiHelper volumeMultiHelper(*physicalVolumesMulti);
	    productionVolumeName = volumeMultiHelper.startVolume(*simparticle).name();
	  }

	//Set the parent id to 0 if no parent, otherwise identify the parent
	int parentPDGid = 0;
	double parentInitX = NAN, parentInitY = NAN, parentInitZ = NAN;
	
	if (simparticle->hasParent()){	      
	  parentPDGid = simparticle->parent()->pdgId();
	  parentInitX = simparticle->parent()->startPosition().x();
	  parentInitY = simparticle->parent()->startPosition().y();
	  parentInitZ = simparticle->parent()->startPosition().z();
	}
	hit_.stepTime                 = stepTime;
	hit_.X                        = CRVHitPosition.x();
	hit_.Y                        = CRVHitPosition.y();
	hit_.Z                        = CRVHitPosition.z();
	hit_.InitX                    = simparticle->startPosition().x();
	hit_.InitY                    = simparticle->startPosition().y();
	hit_.InitZ                    = simparticle->startPosition().z();
	hit_.InitPx                   = simparticle->startMomentum().x();
	hit_.InitPy                   = simparticle->startMomentum().y();
	hit_.InitPz                   = simparticle->startMomentum().z();
	hit_.parentInitX              = parentInitX;
	hit_.parentInitY              = parentInitY;
	hit_.parentInitZ              = parentInitZ;
	hit_.stepEDep                 = stepEDep;
	hit_.totalEDep                = totEDep;
	hit_.visEn                    = totEDep-nonIonEDep;
	hit_.nonIonEDep               = nonIonEDep;
	hit_.momentum                 = steps[ispmc].momentum().mag();
	hit_.px                       = steps[ispmc].momentum().x();
	hit_.py                       = steps[ispmc].momentum().y();
	hit_.pz                       = steps[ispmc].momentum().z();
	hit_.layerLengthX             = 2 * layer.getHalfLengths()[0];
	hit_.layerLengthY             = 2 * layer.getHalfLengths()[1];
	hit_.layerLengthZ             = 2 * layer.getHalfLengths()[2];
	hit_.EventID                  = event.id().event();
	hit_.section                  = Section;
	hit_.sector                   = Sector;
	hit_.numModulesInSection      = CRS->getCRSScintillatorShield(CRVCounterId.getShieldId()).nModules();
	hit_.module                   = Module;
	hit_.layer                    = Layer;
	hit_.counter                  = Counter;
	hit_.barIndex                 = barIndex;
	hit_.PDGid                    = PDGid;
	hit_.parentPDGid              = parentPDGid;
	strncpy(hit_.productionProcessName, productionProcessName.c_str(),99);
	strncpy(hit_.productionVolumeName,  productionVolumeName.c_str() ,99);

	_crvHits->Fill(); //Fill the tree/branch
	lastTrkIndx = trkIndx;
	lastLayer = Layer;
	lastEvNum = evNum;
      }
    }
  }

  int CRVHitAna::getCrvSection(int shieldID)
  {
    int Section = -1;
    // split top plus TST extension
    switch (shieldID)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:  Section = 0; break;  //R
      case 6:
      case 7:
      case 8:  Section = 1; break;  //L
      case 9: 
      case 10: Section = 5; break;  //TST
      case 11:
      case 12: 
      case 13: 
      case 14: Section = 2; break;  //T
      case 15:                    
      case 16: Section = 6; break;  //TST extension
      case 17: Section = 3; break;  //D
      case 18: Section = 4; break;  //U
      case 19: 
      case 20: 
      case 21: Section = 7; break;  //CT
      case 22:
      case 23: Section = 3; break; //D - new
      }
    return Section;
  }

}



using mu2e::CRVHitAna;
DEFINE_ART_MODULE(CRVHitAna);
