#include "TTree.h"
#include "CosmicRayShieldGeom/inc/CosmicRayShield.hh"
#include "DataProducts/inc/CRSScintillatorBarIndex.hh"
#include "ConditionsService/inc/AcceleratorParams.hh"
#include "ConditionsService/inc/ConditionsHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/GeometryService.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "MCDataProducts/inc/StepInstanceName.hh"
#include "MCDataProducts/inc/MCTrajectoryCollection.hh"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Services/Optional/TFileDirectory.h"
#include "fhiclcpp/ParameterSet.h"
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "HepPID/ParticleName.hh"
#include "RecoDataProducts/inc/CrvCoincidenceClusterSummaryCollection.hh"
#include <set>

namespace mu2e{

  struct CoincidenceInfo{
    int EventID, cointype, section[10], pe[10], barIndex[10], arrIndex[10], sipm[10];
    int PDGid[10], parentPDGid[10], parentTrkid[10], primaryPDGid[10], primaryTrkid[10];
    double startTime, endTime;
    double pulseTime[10];
    double mcX, mcY, mcZ, EDep;
    double mcParentX[10], mcParentY[10], mcParentZ[10];
    double mcPrimaryX[10], mcPrimaryY[10], mcPrimaryZ[10];
    double mcPrimaryPx[10], mcPrimaryPy[10], mcPrimaryPz[10];
    char PDGName[10][100], parentPDGName[10][100], primaryPDGName[10][100];
    void clear(){
      EventID = cointype = -999; 
      startTime = endTime = NAN;
      mcX = mcY = mcZ = EDep = NAN;
      for(int i=0; i<10; i++){
	section[i] = pe[i] = barIndex[i] = arrIndex[i] = sipm[i] = -999;
	PDGid[i] = parentPDGid[i] = parentTrkid[i] = primaryPDGid[i] = primaryTrkid[i] = -999;
	pulseTime[i] = NAN;
	mcParentX[i] = mcParentY[i] = mcParentZ[i] = NAN;
	mcPrimaryX[i] = mcPrimaryY[i] = mcPrimaryZ[i] = NAN;
	mcPrimaryPx[i] = mcPrimaryPy[i] = mcPrimaryPz[i] = NAN;
      }
    }
  }; // struct CoincidenceInfo


  class CRVCoincidenceTree : public art::EDAnalyzer
  {
  public:
    explicit CRVCoincidenceTree(fhicl::ParameterSet const &pset);
    virtual ~CRVCoincidenceTree() { }
    virtual void beginJob();
    void endJob();
    void analyze(const art::Event& e);
    int getCrvSection(int shieldID);
    double VisibleEnergyDeposition(int PDGcode, double stepLength, double energyDepositedTotal, double energyDepositedNonIonizing);
    // const art::Ptr<StepPointMC> GetStepPoint(const art::Ptr<CrvRecoPulse> &crvRecoPulse);

  private:
    std::vector<std::string> _crvCoincidences;
    std::string _crvWaveformsModuleLabel;
    double _leadingVetoTime;
    double _trailingVetoTime;
    double  _timeWindow;
    double  _peThresh;
    double  _startTime;
    double  _endTime;
    double  _evEDep;
    int    _verboseLevel;
    SimParticleTimeOffset _timeOffsets;
    TTree*   coinTree;
    CoincidenceInfo coinStruct;
    double tot_dtime, tot_ltime;
  };

  CRVCoincidenceTree::CRVCoincidenceTree(fhicl::ParameterSet const &pset) :
    art::EDAnalyzer(pset),
    _crvCoincidences(pset.get<std::vector<std::string> >("crvCoincidences")),
    _crvWaveformsModuleLabel(pset.get<std::string>("crvWaveformsModuleLabel","")),    
    _leadingVetoTime(pset.get<double>("leadingVetoTime")),
    _trailingVetoTime(pset.get<double>("trailingVetoTime")),
    _timeWindow(pset.get<double>("timeWindow", 20)),
    _peThresh(pset.get<double>("peThresh", 6)),
    _startTime(pset.get<double>("startTime", 550)),
    _endTime(pset.get<double>("endTime", 1695)),
    _verboseLevel(pset.get<int>("verboseLevel")),
    _timeOffsets(pset.get<fhicl::ParameterSet>("timeOffsets", fhicl::ParameterSet()))
  {
  }

  void CRVCoincidenceTree::beginJob(){
    tot_dtime = tot_ltime = 0.0;
    CoincidenceInfo &c = coinStruct;
    art::ServiceHandle<art::TFileService> tfs;
    coinTree = tfs->make<TTree>("coincidenceCombinations", "Coincidence Information Tree");
    coinTree->Branch("startTime", &c.startTime, "startTime/D");
    coinTree->Branch("endTime", &c.endTime, "endTime/D");
    coinTree->Branch("barIndex", &c.barIndex, "barIndex[10]/I");
    coinTree->Branch("arrIndex", &c.arrIndex, "arrIndex[10]/I");
    coinTree->Branch("sipm", &c.sipm, "sipm[10]/I");
    coinTree->Branch("section", &c.section, "section[10]/I");
    coinTree->Branch("EventID", &c.EventID, "EventID/I");
    coinTree->Branch("cointype",&c.cointype,"cointype/I");
    coinTree->Branch("pe",c.pe,"pe[10]/I");
    coinTree->Branch("pulseTime",c.pulseTime,"pulseTime[10]/D");
    coinTree->Branch("mcX",&c.mcX,"mcX/D");
    coinTree->Branch("mcY",&c.mcY,"mcY/D");
    coinTree->Branch("mcZ",&c.mcZ,"mcZ/D");
    coinTree->Branch("EDep",&c.EDep,"EDep/D");
    coinTree->Branch("mcParentX",c.mcParentX,"mcParentX[10]/D");
    coinTree->Branch("mcParentY",c.mcParentY,"mcParentY[10]/D");
    coinTree->Branch("mcParentZ",c.mcParentZ,"mcParentZ[10]/D");
    coinTree->Branch("mcPrimaryX",c.mcPrimaryX,"mcPrimaryX[10]/D");
    coinTree->Branch("mcPrimaryY",c.mcPrimaryY,"mcPrimaryY[10]/D");
    coinTree->Branch("mcPrimaryZ",c.mcPrimaryZ,"mcPrimaryZ[10]/D");

    coinTree->Branch("mcPrimaryPx",c.mcPrimaryPx,"mcPrimaryPx[10]/D");
    coinTree->Branch("mcPrimaryPy",c.mcPrimaryPy,"mcPrimaryPy[10]/D");
    coinTree->Branch("mcPrimaryPz",c.mcPrimaryPz,"mcPrimaryPz[10]/D");

    coinTree->Branch("PDGid",c.PDGid,"PDGid[10]/I");
    coinTree->Branch("parentPDGid",c.parentPDGid,"parentPDGid[10]/I");
    coinTree->Branch("parentTrkid",c.parentTrkid,"parentTrkid[10]/I");
    coinTree->Branch("primaryPDGid",c.primaryPDGid,"primaryPDGid[10]/I");
    coinTree->Branch("primaryTrkid",c.primaryTrkid,"primaryTrkid[10]/I");
    coinTree->Branch("PDGName",c.PDGName,"PDGName[10][100]/C");
    coinTree->Branch("parentPDGName",c.parentPDGName,"parentPDGName[10][100]/C");
    coinTree->Branch("primaryPDGName",c.primaryPDGName,"primaryPDGName[10][100]/C");
  }

  void CRVCoincidenceTree::endJob(){}

  void CRVCoincidenceTree::analyze(const art::Event& event){
    std::cout << "----- Processing event:" << event.id().event() << " -----" << std::endl;
    // Time offsets from proton pulse shape and stopped muons. Offsets are applied to step points
    _timeOffsets.updateMap(event);
    //This selector will select only data products with the given instance name
    std::unique_ptr<art::Selector> selector;
    typedef std::vector< art::Handle<StepPointMCCollection> > HandleVector;
    HandleVector stepsHandles;   
    selector = std::unique_ptr<art::Selector>(new art::Selector(art::ProductInstanceNameSelector("CRV")));    
    event.getMany(*selector, stepsHandles); //Get all of the CRV StepPointMC collections from the event
    //If stepsHandles was empty, then there were no StepPointMCs relating to the CRV
    if (stepsHandles.empty())      
      throw cet::exception("SIM") << "mu2e: CRVCoincidenceTree: No StepPointMC collections found for CRV" << std::endl;      
    //Geometry handle
    GeomHandle<CosmicRayShield> CRS;

    //-------------------------------------------------------------------------------------------------------
    //		Fill an array of coincidences
    //-------------------------------------------------------------------------------------------------------    
    for (unsigned int label = 0; label < _crvCoincidences.size(); label++){ // get the coincidence check for each label

      art::Handle<CrvCoincidenceClusterSummaryCollection> crvCoincidenceClusterSummaryCollection;
      event.getByLabel(_crvCoincidences[label],"",crvCoincidenceClusterSummaryCollection);
      if(crvCoincidenceClusterSummaryCollection.product()==NULL) return;

      std::vector<CrvCoincidenceClusterSummary>::const_iterator iter;
      tot_ltime = tot_ltime + _endTime - _startTime;
      
      for(iter=crvCoincidenceClusterSummaryCollection->begin(); iter!=crvCoincidenceClusterSummaryCollection->end(); iter++){
	const CrvCoincidenceClusterSummary &cluster = *iter;
	coinStruct.clear();
	double startTime =  cluster.GetStartTime();
	double endTime   =  cluster.GetEndTime() + _trailingVetoTime;
	if(endTime < _startTime) continue;

	std::cout<< "Start time: " << startTime << ", End time: " << endTime << ", in sector:" << cluster.GetCrvSectorType() << std::endl;
	if(startTime < _startTime) startTime = _startTime;   // Set to beginning of signal window
	if(endTime > _endTime)   endTime = _endTime;         // Set to end of signal window 
	tot_dtime = tot_dtime + endTime - startTime;
	std::cout << "Total dead time: " << 100*tot_dtime/tot_ltime << std::endl;
	std::vector<CrvCoincidenceClusterSummary::PulseInfo> pulses = cluster.GetPulses();
	std::cout<< pulses.size() << std::endl;

	std::set<int> seen_bar;
	int ihit = 0;
	for(unsigned int j=0; j<pulses.size(); j++){
	  if (ihit>=10) break;
	  int baridx = pulses[j]._crvRecoPulse->GetScintillatorBarIndex().asInt();	  	  
	  if(seen_bar.find(baridx) != seen_bar.end())
	    continue;
	  // New bar has a hit
	  seen_bar.insert(pulses[j]._crvRecoPulse->GetScintillatorBarIndex().asInt());
	  ihit++;
	  

	  std::cout<< pulses[j]._crvRecoPulse->GetPEs() << " "
		   << pulses[j]._crvRecoPulse->GetScintillatorBarIndex().asInt()<< " "
		   << pulses[j]._simParticle->pdgId()<< " "
		   << pulses[j]._simParticle->parent()->pdgId()<< " "
		   << std::endl;

	  coinStruct.pe[ihit]            = pulses[j]._crvRecoPulse->GetPEs();
	  coinStruct.pulseTime[ihit]     = pulses[j]._crvRecoPulse->GetPulseTime();
	  coinStruct.barIndex[ihit]      = pulses[j]._crvRecoPulse->GetScintillatorBarIndex().asInt();
	  coinStruct.arrIndex[ihit]      = j;
	  const CRSScintillatorBar &CRSbar = CRS->getBar(pulses[j]._crvRecoPulse->GetScintillatorBarIndex());
	  coinStruct.section[ihit]       = getCrvSection(CRSbar.id().getShieldNumber());
	  coinStruct.sipm[ihit]          = pulses[j]._crvRecoPulse->GetSiPMNumber();
	  coinStruct.PDGid[ihit]         = pulses[j]._simParticle->pdgId();
	  strncpy(coinStruct.PDGName[ihit], HepPID::particleName(coinStruct.PDGid[ihit]).c_str(),99);
	  coinStruct.parentPDGid[ihit]   = pulses[j]._simParticle->parent()->pdgId();
	  strncpy(coinStruct.parentPDGName[ihit], HepPID::particleName(coinStruct.parentPDGid[ihit]).c_str(),99);

	  // Fill parent info
	  art::Ptr<mu2e::SimParticle> parent = pulses[j]._simParticle;
	  if(parent->hasParent()){
	    parent = parent->parent();	  
	    coinStruct.parentTrkid[ihit] = parent->id().asInt();
	    coinStruct.mcParentX[ihit]   = parent->startPosition().x();
	    coinStruct.mcParentY[ihit]   = parent->startPosition().y();
	    coinStruct.mcParentZ[ihit]   = parent->startPosition().z();
	  }


	  //Find and save primary track info
	  art::Ptr<mu2e::SimParticle> primary = pulses[j]._simParticle;
	  while(primary->creationCode() != ProcessCode::mu2ePrimary && primary->hasParent()){
	    std::cout<< " "
		     << primary->id().asInt() << " "
		     << primary->parent()->id().asInt()	 << " "   
		     << primary->parent()->startMomentum().vect().mag() << " "
		     << primary->parent()->pdgId() << " "
		     << std::endl;
	    primary = primary->parent();	  
	  }
	  coinStruct.primaryTrkid[ihit] = primary->id().asInt();
	  coinStruct.primaryPDGid[ihit] = primary->pdgId();
	  strncpy(coinStruct.primaryPDGName[ihit], HepPID::particleName(primary->pdgId()).c_str(),99);
	  coinStruct.mcPrimaryX[ihit]   = primary->startPosition().x();
	  coinStruct.mcPrimaryY[ihit]   = primary->startPosition().y();
	  coinStruct.mcPrimaryZ[ihit]   = primary->startPosition().z();	  
	  coinStruct.mcPrimaryPx[ihit]  = primary->startMomentum().x();
	  coinStruct.mcPrimaryPy[ihit]  = primary->startMomentum().y();
	  coinStruct.mcPrimaryPz[ihit]  = primary->startMomentum().z();
	
	  
	  for (const auto& elem: seen_bar) {
	    std::cout<< elem << std::endl;	  
	  }
	  	  
	  // Loop through unique bars only
	  // for(unsigned int j=0; j<unique_hits.size(); j++){
	  // 	int hit_idx = unique_hits[j];	  
	  // 	if(hit_idx>=10) break; // Maximum number of hits per coincidence cluster	 	  
	  // 	if (std::find(seen_trk.begin(), seen_trk.end(), primary->id().asInt()) == seen_trk.end())
	  // 	  seen_trk.push_back(primary->id().asInt());
	  // }
	  // coinStruct.cointype = seen_trk.size();
	  
	  coinStruct.startTime = startTime;
	  coinStruct.endTime   = endTime;
	  coinStruct.EventID   = event.id().event();
	  coinStruct.mcX       = cluster.GetEarliestHitPos().x();
	  coinStruct.mcY       = cluster.GetEarliestHitPos().y();
	  coinStruct.mcZ       = cluster.GetEarliestHitPos().z();
	  coinStruct.EDep      = cluster.GetTotalEnergyDeposited();	  
	  coinTree->Fill();
	}
      }
    }
  }

  int CRVCoincidenceTree::getCrvSection(int shieldID){
    int Section = -1;
    switch (shieldID){
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:  Section = 0; break;  //R
      case 5:  
      case 6:  Section = 1; break;  //L
      case 7:
      case 8: 
      case 9: 
      case 10: Section = 2; break;  //T
      case 11:
      case 12: Section = 6; break;  //E
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

  double CRVCoincidenceTree::VisibleEnergyDeposition(int PDGcode, double stepLength,
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

}

using mu2e::CRVCoincidenceTree;
DEFINE_ART_MODULE(CRVCoincidenceTree);
