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
#include "MCDataProducts/inc/GenParticle.hh"
#include "MCDataProducts/inc/GenParticleCollection.hh"
#include "MCDataProducts/inc/PhysicalVolumeInfoMultiCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "MCDataProducts/inc/StepInstanceName.hh"
#include "MCDataProducts/inc/MCTrajectoryCollection.hh"
#include "MCDataProducts/inc/CrvDigiMCCollection.hh"
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
#include "RecoDataProducts/inc/CrvRecoPulseCollection.hh"
#include "HepPID/ParticleName.hh"

namespace mu2e
{

  const double nan = std::numeric_limits<double>::quiet_NaN();

  //Structure to contain hit information for particles for filling tree
  struct CRVHit{
    double pulseTime, stepTime;
    double Pos[3], P[3], InitPos[3], InitP[3], ParentPos[3], PrimPos[3], PrimP[3];
    double stepEDep, totalEDep, nonIonEDep, visEn;
    double momentum, px, py, pz;
    int    EventID, sipm, peInt, peMax;
    int    section, numModulesInSection, module, sector;
    int    layer, counter, barIndex;
    int    trkIndx, PrimTrkid, PDGid, ParentPDGid, PrimPDGid;
    char   productionVolumeName[100], productionProcessName[100];
    char   PDGName[100], ParentPDGName[100], PrimPDGName[100];
  }; // struct CRVHit
  
  class CRVPulseTree : public art::EDAnalyzer
  {
  public:
    explicit CRVPulseTree(fhicl::ParameterSet const &pset);
    virtual ~CRVPulseTree() { }
    virtual void beginJob();
    void endJob();
    void analyze(const art::Event& e);
    int getCrvSection(int shieldID);
    void CollectStepPoints(std::set<art::Ptr<StepPointMC> > &steps, 
                           const art::Handle<CrvDigiMCCollection> &digis, 
                           size_t waveformIndex);
    void GetSummary(const std::set<art::Ptr<StepPointMC> > &steps, 
		    double &energyDeposited,
		    StepPointMC &mostLikelyStepPointMC,
		    art::Ptr<SimParticle> &mostLikelySimParticle);
    double VisibleEnergyDeposition(int PDGcode, double stepLength,
				   double energyDepositedTotal,
				   double energyDepositedNonIonizing);


  private:

    std::string _volumeModuleLabel;
    std::string _processName;
    std::string _crvWaveformsModuleLabel;
    double   _leadingVetoTime;
    double   _trailingVetoTime;
    double   _timeWindow;
    double   _peThresh;
    double   _startTime;
    double   _endTime;
    int      _verboseLevel;
    TTree*   _pulseTree;
    CRVHit   _hit; //Struct to act as a buffer for filling tree with hit information
  };

  CRVPulseTree::CRVPulseTree(fhicl::ParameterSet const &pset) :
    art::EDAnalyzer(pset),
    _volumeModuleLabel(pset.get<std::string>("volumeModuleLabel")),
    _processName(pset.get<std::string>("processName")),
    _crvWaveformsModuleLabel(pset.get<std::string>("crvWaveformsModuleLabel","")),
    _leadingVetoTime(pset.get<double>("leadingVetoTime")),
    _trailingVetoTime(pset.get<double>("trailingVetoTime")),
    _timeWindow(pset.get<double>("timeWindow", 20)),
    _peThresh(pset.get<double>("peThresh", 5)),
    _startTime(pset.get<double>("startTime", 550)),
    _endTime(pset.get<double>("endTime", 1695)),
    _verboseLevel(pset.get<int>("verboseLevel"))
  {
  }

  void CRVPulseTree::beginJob()
  {
    CRVHit &p = _hit;
    art::ServiceHandle<art::TFileService> tfs;
    _pulseTree = tfs->make<TTree>("crvPulses", "CRV Pulses Tree"); //Make second tree
    _pulseTree->Branch("pulseTime", &p.pulseTime, "pulseTime/D");
    _pulseTree->Branch("peMax", &p.peMax, "peMax/I");
    _pulseTree->Branch("peInt", &p.peInt, "peInt/I");
    _pulseTree->Branch("sipm", &p.sipm, "sipm/I");
    _pulseTree->Branch("barIndex", &p.barIndex, "barIndex/I");
    _pulseTree->Branch("section", &p.section, "section/I");
    _pulseTree->Branch("sector", &p.sector, "sector/I");
    _pulseTree->Branch("EventID", &p.EventID, "EventID/I");
    _pulseTree->Branch("Pos", &p.Pos, "Pos[3]/D");
    _pulseTree->Branch("InitPos", &p.InitPos, "InitPos[3]/D");
    _pulseTree->Branch("PrimPos", &p.PrimPos, "PrimPos[3]/D");
    _pulseTree->Branch("ParentPos", &p.ParentPos, "ParentPos[3]/D");
    _pulseTree->Branch("P", &p.P, "P[3]/D");
    _pulseTree->Branch("InitP", &p.InitP, "InitP[3]/D");
    _pulseTree->Branch("PrimP", &p.PrimP, "PrimP[3]/D");
    _pulseTree->Branch("stepEDep", &p.stepEDep, "stepEDep/D");
    _pulseTree->Branch("totalEDep", &p.totalEDep, "totalEDep/D");
    _pulseTree->Branch("nonIonEDep", &p.nonIonEDep, "nonIonEDep/D");
    _pulseTree->Branch("PDGid", &p.PDGid, "PDGid/I");
    _pulseTree->Branch("ParentPDGid", &p.ParentPDGid, "ParentPDGid/I");
    _pulseTree->Branch("PrimPDGid", &p.PrimPDGid, "PrimPDGid/I");
    _pulseTree->Branch("productionVolumeName", &p.productionVolumeName, "productionVolumeName[100]/C");
    _pulseTree->Branch("productionProcessName", &p.productionProcessName, "productionProcessName[100]/C");
    _pulseTree->Branch("PDGName",&p.PDGName,"PDGName[100]/C");
    _pulseTree->Branch("ParentPDGName",&p.ParentPDGName,"ParentPDGName[100]/C");
    _pulseTree->Branch("PrimPDGName",&p.PrimPDGName,"PrimPDGName[100]/C");
    _pulseTree->Branch("PrimTrkid",&p.PrimTrkid,"PrimTrkid/I");
  }

  void CRVPulseTree::endJob()
  {
  }

  void CRVPulseTree::analyze(const art::Event& event)
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
      throw cet::exception("SIM") << "mu2e: CRVPulseTreelysis-CRVPulseTree: No StepPointMC collections found for CRV" << std::endl;
      
    //Volume information
    art::Handle<mu2e::PhysicalVolumeInfoMultiCollection> physicalVolumesMulti;
    bool hasPhysicalVolumes = event.getSubRun().getByLabel(_volumeModuleLabel, physicalVolumesMulti);
    //Geometry handle
    GeomHandle<CosmicRayShield> CRS;

    std::cout << "----- Processing event:" << event.id().event() << " -----" << std::endl;
    //-------------------------------------------------------------------------------------------------------
    //		Get RecoPulses, make associations, and fill association tree
    //-------------------------------------------------------------------------------------------------------

    art::Handle<CrvRecoPulseCollection> crvRecoPulseCollection;
    event.getByLabel("CrvRecoPulses","",crvRecoPulseCollection);
    assert(crvRecoPulseCollection.isValid() == false);
    
    //loop over reco pulse collection (=loop over counters)
    for(size_t recoPulseIndex=0; recoPulseIndex<crvRecoPulseCollection->size(); recoPulseIndex++){

      const art::Ptr<CrvRecoPulse> crvRecoPulse(crvRecoPulseCollection, recoPulseIndex);
      //get information about the counter
      const CRSScintillatorBarIndex &barIndex = crvRecoPulse->GetScintillatorBarIndex();
      const CRSScintillatorBar &CRSbar = CRS->getBar(barIndex);
      const CRSScintillatorBarId &barId = CRSbar.id();
      int sectorNumber  = barId.getShieldNumber();
      int sectionNumber = getCrvSection(barId.getShieldNumber());
      int moduleNumber  = barId.getModuleNumber();
      int layerNumber   = barId.getLayerNumber();
      int barNumber     = barId.getBarNumber();   //counter number within this sector, module, and layer
      int barIndx      = CRSbar.index().asInt();

      
 // Reset all for new pulse
      double stepEDep, totEDep, nonIonEDep, visEn, stepTime;
      int trkIndx, PrimtrkIndx, PDGid, ParentPDGid, PrimPDGid; 
      trkIndx = PrimtrkIndx = PDGid = ParentPDGid = PrimPDGid = stepEDep = stepTime = -9999;
      totEDep = nonIonEDep = visEn = 0;
      double Pos[3]       = {NAN, NAN, NAN};
      double InitPos[3]   = {NAN, NAN, NAN};
      double ParentPos[3] = {NAN, NAN, NAN};
      double PrimPos[3]   = {NAN, NAN, NAN};
      double P[3]         = {NAN, NAN, NAN};
      double InitP[3]     = {NAN, NAN, NAN};
      double PrimP[3]     = {NAN, NAN, NAN};
      std::string productionVolumeName  = "N/A";
      std::string productionProcessName = "N/A";          
      double pulseTime  = crvRecoPulse->GetPulseTime();
      int peInt = crvRecoPulse->GetPEs();
      int peMax = crvRecoPulse->GetPEsPulseHeight();
      int SiPM  = crvRecoPulse->GetSiPMNumber();


      
      //get the CrvDigiMC collection
      art::Handle<CrvDigiMCCollection> crvDigiMCCollection;
      event.getByLabel(_crvWaveformsModuleLabel,"",crvDigiMCCollection); //this is an optional part for MC information
      assert(crvDigiMCCollection.isValid() == false);

      art::Ptr<SimParticle> simParticle;
      StepPointMC stepPointMC;
      
      //get MC information, if available
      std::set<art::Ptr<StepPointMC> > stepsThisPulse;
      const std::vector<size_t> &waveformIndices = crvRecoPulse->GetWaveformIndices();
      for(size_t j=0; j<waveformIndices.size(); j++) {
	size_t waveformIndex = waveformIndices[j];
	CollectStepPoints(stepsThisPulse, crvDigiMCCollection, waveformIndex);
      }	  
      GetSummary(stepsThisPulse, totEDep, stepPointMC, simParticle);            
      PDGid    = simParticle->pdgId();   
      Pos[0]   = stepPointMC.position().x();
      Pos[1]   = stepPointMC.position().y();
      Pos[2]   = stepPointMC.position().z();
      P[0]     = stepPointMC.momentum().x();
      P[1]     = stepPointMC.momentum().y();
      P[2]     = stepPointMC.momentum().z();
      stepTime = stepPointMC.time();	  

      
      //this sim particle may have been created in the previous simulation stage
      //therefore, a particle which has the creation code "primary", may not be a primary
      //but a continuation of a particle from a previous stage.
      //this particle from the previous stage is this particle's parent
      if (simParticle->creationCode() == ProcessCode::mu2ePrimary && simParticle->hasParent()) 
	simParticle = simParticle->parent();
      
      ProcessCode productionProcess = simParticle->creationCode();
      productionProcessName = productionProcess.name();
      
      if (hasPhysicalVolumes && physicalVolumesMulti.product() != NULL){
	mu2e::PhysicalVolumeMultiHelper volumeMultiHelper(*physicalVolumesMulti);
	productionVolumeName = volumeMultiHelper.startVolume(*simParticle).name();
      }
      
      if (simParticle->hasParent()){	      
	ParentPDGid  = simParticle->parent()->pdgId();
	ParentPos[0] = simParticle->parent()->startPosition().x();
	ParentPos[1] = simParticle->parent()->startPosition().y();
	ParentPos[2] = simParticle->parent()->startPosition().z();
      }	      
      InitPos[0] = (double) simParticle->startPosition().x();
      InitPos[1] = (double) simParticle->startPosition().y();
      InitPos[2] = (double) simParticle->startPosition().z();	      
      InitP[0]   = (double) simParticle->startMomentum().x();
      InitP[1]   = (double) simParticle->startMomentum().y();
      InitP[2]   = (double) simParticle->startMomentum().z();


      //Find and save primary track info
      art::Ptr<mu2e::SimParticle> primary = simParticle;
      while(primary->creationCode() != ProcessCode::mu2ePrimary && primary->hasParent())
	primary = primary->parent();
      
      PrimtrkIndx = primary->id().asInt();
      PrimPDGid   = primary->pdgId();

      PrimPos[0] = primary->startPosition().x();
      PrimPos[1] = primary->startPosition().y();
      PrimPos[2] = primary->startPosition().z();	  
      PrimP[0]   = primary->startMomentum().x();
      PrimP[1]   = primary->startMomentum().y();
      PrimP[2]   = primary->startMomentum().z();

    	          
      _hit.pulseTime                = pulseTime;
      _hit.peMax                    = peMax;
      _hit.peInt                    = peInt;
      _hit.sipm                     = SiPM;
      _hit.stepTime                 = stepTime;
      _hit.Pos[0]                   = Pos[0];
      _hit.Pos[1]                   = Pos[1];
      _hit.Pos[2]                   = Pos[2];
      _hit.P[0]                     = P[0];
      _hit.P[1]                     = P[1];
      _hit.P[2]                     = P[2];
      _hit.InitPos[0]               = InitPos[0];
      _hit.InitPos[1]               = InitPos[1];
      _hit.InitPos[2]               = InitPos[2];
      _hit.InitP[0]                 = InitP[0];
      _hit.InitP[1]                 = InitP[1];
      _hit.InitP[2]                 = InitP[2];
      _hit.PrimPos[0]               = PrimPos[0];
      _hit.PrimPos[1]               = PrimPos[1];
      _hit.PrimPos[2]               = PrimPos[2];
      _hit.PrimP[0]                 = PrimP[0];
      _hit.PrimP[1]                 = PrimP[1];
      _hit.PrimP[2]                 = PrimP[2];
      _hit.ParentPos[0]             = ParentPos[0];
      _hit.ParentPos[1]             = ParentPos[1];
      _hit.ParentPos[2]             = ParentPos[2];
      _hit.stepEDep                 = stepEDep;
      _hit.totalEDep                = totEDep;
      _hit.visEn                    = totEDep-nonIonEDep;
      _hit.nonIonEDep               = nonIonEDep;
      _hit.EventID                  = event.id().event();
      _hit.section                  = sectionNumber;
      _hit.sector                   = sectorNumber;
      _hit.numModulesInSection      = CRS->getCRSScintillatorShield(barId.getShieldId()).nModules();
      _hit.module                   = moduleNumber;
      _hit.layer                    = layerNumber;
      _hit.counter                  = barNumber;
      _hit.barIndex                 = barIndx;
      _hit.trkIndx                  = trkIndx;
      _hit.PDGid                    = PDGid;
      _hit.PrimTrkid                = PrimtrkIndx;
      _hit.ParentPDGid              = ParentPDGid;
      _hit.PrimPDGid                = PrimPDGid;
      strncpy(_hit.productionProcessName, productionProcessName.c_str(),99);
      strncpy(_hit.productionVolumeName,  productionVolumeName.c_str() ,99);
      strncpy(_hit.PrimPDGName, HepPID::particleName(PrimPDGid).c_str(),99);	  
      strncpy(_hit.ParentPDGName, HepPID::particleName(ParentPDGid).c_str(),99);	  
      strncpy(_hit.PDGName, HepPID::particleName(PDGid).c_str(),99);
      _pulseTree->Fill();
            
    } // Loop over SiPM in recoPulse
  }

  double CRVPulseTree::VisibleEnergyDeposition(int PDGcode, double stepLength,
						  double energyDepositedTotal,
						  double energyDepositedNonIonizing){
    if(energyDepositedTotal <= 0.0) { return 0.0; }
    const double _scintillatorBirksConstant = 0.126;
    double evis = energyDepositedTotal;
    if(PDGcode==22){
      if(evis>0){
	double eDepOverElectronRange = 27.0*exp(-0.247*pow(fabs(log(evis)+8.2),1.6))+0.177;
	evis /= (1.0 + _scintillatorBirksConstant*eDepOverElectronRange);
      }
    }
    else {
      // protections
      double nloss = energyDepositedNonIonizing;
      if(nloss < 0.0) nloss = 0.0;
      double eloss = energyDepositedTotal - nloss;      
      // neutrons
      if(PDGcode==2112 || eloss < 0.0 || stepLength <= 0.0){
	nloss = energyDepositedTotal;
	eloss = 0.0;
      }      
      // continues energy loss
      if(eloss > 0.0) { eloss /= (1.0 + _scintillatorBirksConstant*eloss/stepLength);}      
      evis = eloss + nloss;
    }
    return evis;
  }

  int CRVPulseTree::getCrvSection(int shieldID)
  {
    int Section = -1;
    // split top plus TST extension
    switch (shieldID)
      {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:  Section = 0; break;  //R
      case 5:  
      case 6:  Section = 1; break;  //L
      case 7:
      case 8:  Section = 5; break;  //TST
      case 9: 
      case 10: Section = 2; break;  //T
      case 11:
      case 12: Section = 6; break;  //TST extension
      case 13: Section = 4; break;  //U
      case 14: 
      case 15:                    
      case 16: 
      case 17: Section = 3; break;  //D
      case 18: 
      case 19: 
      case 20: 
      case 21: Section = 7; break;  //CT
      }
    return Section;
  }


  void CRVPulseTree::CollectStepPoints(std::set<art::Ptr<StepPointMC> > &steps, 
				       const art::Handle<CrvDigiMCCollection> &digis, 
				       size_t waveformIndex)
  {
    const CrvDigiMC &digi = digis->at(waveformIndex);
    const std::vector<art::Ptr<StepPointMC> > &stepPoints = digi.GetStepPoints();
    for(size_t k=0; k<stepPoints.size(); k++)
    {
      if(stepPoints[k].isNonnull()) steps.insert(stepPoints[k]);
    }
  }

  void CRVPulseTree::GetSummary(const std::set<art::Ptr<StepPointMC> > &steps, 
				double &energyDeposited,
				StepPointMC &mostLikelyStepPointMC,
				art::Ptr<SimParticle> &mostLikelySimParticle){
    energyDeposited=0;
    double mostDeposited=NAN;    
    std::map<art::Ptr<SimParticle>,double> simParticleMap;
    std::set<art::Ptr<StepPointMC> >::const_iterator i;
    for(i=steps.begin(); i!=steps.end(); i++){
      const StepPointMC &step = **i;
      energyDeposited+=step.totalEDep();
      simParticleMap[step.simParticle()]+=step.totalEDep();
      if(isnan(mostDeposited) || mostDeposited<step.totalEDep()){
	mostLikelyStepPointMC=step;
      }
    }

    std::map<art::Ptr<SimParticle>,double>::iterator simParticleIter;
    double simParticleDepEnergy=0;
    for(simParticleIter=simParticleMap.begin(); simParticleIter!=simParticleMap.end(); simParticleIter++){
      if(simParticleIter->second>simParticleDepEnergy){
        simParticleDepEnergy=simParticleIter->second;
        mostLikelySimParticle=simParticleIter->first;
      }
    }

  }


}



using mu2e::CRVPulseTree;
DEFINE_ART_MODULE(CRVPulseTree);
