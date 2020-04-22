//Produced standard plots from a TrkAna Tree
//Based off plotCRY_standarized.C
//Ben Barton
//Jan 2020


/*********************************************************************************************************************************************************/
///////////////////////////////////////////////////////////    Imports    /////////////////////////////////////////////////////////////////////////////////

//Root headers
#include <TH1F.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TROOT.h>
#include <TCut.h>
#include <TStyle.h>
#include <TFile.h>

//C & C++ headers
#include <string>

/*********************************************************************************************************************************************************/
////////////////////////////////////////////////////////   Control Params    //////////////////////////////////////////////////////////////////////////////

static bool INC_TRKQUAL_SFX = false; //Prior to mid-July 2019, dequal leaves were named with suffixes - set to true to read trees prior to this date
static bool INC_CRV_SUMMARIES = true; //Set to true if examining tree with Ralf's CRV summary leaves 

/*********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Define Standard Histograms & Graphs    /////////////////////////////////////////////////////////////

TH1F *h_crvinfomc_x0;
TH1F *h_crvinfomc_y0;
TH1F *h_crvinfomc_z0;
TH2F *h_crvinfomc_z0_vs_x0;
TH2F *h_crvinfomc_z0_vs_x0_noCoinc; 
TH2F *h_crvinfomc_z0_vs_y0;
TH1F *h_crvinfomc_primaryX;
TH1F *h_crvinfomc_primaryY;
TH1F *h_crvinfomc_primaryZ;
TH2F *h_crvinfomc_primaryZ_vs_X;
TH1F *h_crvinfomc_primaryPdgId;
TH1F *h_crvinfomc_primaryE;
TH1F *h_deent_mom;
TH1F *h_deent_d0;
TH1F *h_deent_td;
TH1F *h_de_t0;
TH1F *h_dequal_trkQualDeM;
TH1F *h_demcent_d0;
TH2F *h_det0_vs_CRVtimeWindowStart;
TH2F *h_det0_vs_crvinfomc_time;
TH1F *h_delta_det0_CRVtimeWindowStart;
TH1F *h_delta_det0_crvinfomc_time;
TH1F *h_pz_p;
TH1F *h_noCRV_demc_pdg;
TH2F *h_theta_vs_tandip;
TH2F *h_delta_t_vs_z;
TH1F *h_demcgen_t0;
TH1F *h_demcpri_t0;
TH1F *h_delta_t_minus_demcpri_t0;
TH1F *h_de_t0_minus_demcpri_t0;
TH1F *h_neutronKE;
TH1F *h_neutronKE_noCRV;
TH1F *h_crvsummarymc_total_E_dep;
TH1F *h_nHitCounters;
//TH2F *h_crvinfomcplane_primaryZ_vs_X;


TGraph *g_crvinfomc_z0_vs_x0; 
TGraph *g_crvinfomc_z0_vs_y0;
TGraph *g_crvinfomc_primaryZ_vs_X;
TGraph *g_det0_vs_CRVtimeWindowStart;
TGraph *g_det0_vs_crvinfomc_time;
TGraph *g_cos_theta_vs_tandip;
TGraph *g_oposx_vs_oposz;
TGraph *g_oposy_vs_oposz;
TGraph *g_oposy_vs_oposx;
TGraph *g_crvinfomcplane_primaryZ_vs_X;



void initializeHists(bool makeCuts)
{
  //Position at CRV
  //crvinfomc._x[0] - x position at the CRV
  h_crvinfomc_x0 = new TH1F("h_crvinfomc_x0", "crvinfomc._x[0]", 100, -7.2, 0); 
  h_crvinfomc_x0->SetXTitle("x at CRV (m)");

  //crvinfomc._y[0] - y position at the CRV
  h_crvinfomc_y0 = new TH1F("h_crvinfomc_y0", "crvinfomc._y[0]", 100, -2, 3.2);
  h_crvinfomc_y0->SetXTitle("y at CRV (m)");

  //crvinfomc._z[0] - z position at the CRV
  h_crvinfomc_z0 = new TH1F("h_crvinfomc_z0", "crvinfomc._z[0]", 100, -3, 19);
  h_crvinfomc_z0->SetXTitle("z at CRV (m)");

  //crvinfomc._z[0]:crvinfomc._x[0] - z vs x position at the CRV
  h_crvinfomc_z0_vs_x0 = new TH2F("h_crvinfomc_z0_vs_x0","crvinfomc._z[0] vs crvinfomc._x[0]", 100, -5, 20, 100, -7.5, 1);
  h_crvinfomc_z0_vs_x0->SetXTitle("z at the CRV (m)");
  h_crvinfomc_z0_vs_x0->SetYTitle("x at the CRV (m)");
  h_crvinfomc_z0_vs_x0->SetStats(false);

  h_crvinfomc_z0_vs_x0_noCoinc = new TH2F("h_crvinfomc_z0_vs_x0_noCoinc","crvinfomc._z[0] vs crvinfomc._x[0]", 100, -5, 20, 100, -7.5, 1);
  h_crvinfomc_z0_vs_x0_noCoinc->SetXTitle("z at the CRV (m)");
  h_crvinfomc_z0_vs_x0_noCoinc->SetYTitle("x at the CRV (m)");
  h_crvinfomc_z0_vs_x0_noCoinc->SetStats(false);

  //crvinfomc._z[0]:crvinfomc._y[0] - z vs y position at the CRV
  h_crvinfomc_z0_vs_y0 = new TH2F("h_crvinfomc_z0_vs_y0","crvinfomc._z[0] vs crvinfomc._y[0]", 100, -5, 20, 100, -2., 3.5);
  h_crvinfomc_z0_vs_y0->SetXTitle("z at the CRV (m)");
  h_crvinfomc_z0_vs_y0->SetYTitle("y at the CRV (m)");
  h_crvinfomc_z0_vs_y0->SetStats(false);

  //Primary Particle
  //crvinfomc._primaryX - x position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryX = new TH1F("crvinfomc_primaryX", "crvinfomc._primaryX", 100, -80, 80);
  else
    h_crvinfomc_primaryX = new TH1F("crvinfomc_primaryX", "crvinfomc._primaryX", 100, -80, 80);
  h_crvinfomc_primaryX->SetXTitle("x (m)");

  //crvinfomc._primaryY - y position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryY = new TH1F("crvinfomc_primaryY", "crvinfomc._primaryY", 100, 15.364, 15.410);
  else
    h_crvinfomc_primaryY = new TH1F("crvinfomc_primaryY", "crvinfomc._primaryY", 100, 15.364, 15.410);
  h_crvinfomc_primaryY->SetXTitle("y (m)");

  //crvinfomc._primaryZ - z position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryZ = new TH1F("crvinfomc_primaryZ", "crvinfomc._primaryZ", 100, -80, 80);
  else
    h_crvinfomc_primaryZ = new TH1F("crvinfomc_primaryZ", "crvinfomc._primaryZ", 100, -80, 80);
  h_crvinfomc_primaryZ->SetXTitle("z (m)");

  //cvinfomc._primaryZ:crvinfomc._primaryX - z vs x position of the primary
  h_crvinfomc_primaryZ_vs_X = new TH2F("h_crvinfomc_primaryZ_vs_X", "crvinfomc._primaryZ vs crvinfomc._primaryX", 100, -80, 80, 100, -80, 80);
  h_crvinfomc_primaryZ_vs_X->SetXTitle("z (m)");
  h_crvinfomc_primaryZ_vs_X->SetYTitle("x (m)");
  h_crvinfomc_primaryZ_vs_X->SetStats(false);

  //crvinfomc._primaryPgdId - Primary particle ID
  //if (makeCuts)
  // h_crvinfomc_primaryPdgId = new TH1F("h_crvinfomc_primaryPdgId", "crvinfomc._primaryPdgId", 100, -225, 25);
  //else
  h_crvinfomc_primaryPdgId = new TH1F("h_crvinfomc_primaryPdgId", "crvinfomc._primaryPdgId", 100, -225, 2300);
  h_crvinfomc_primaryPdgId->SetXTitle("Primary Particle PDG ID");

  //crvinfomc._primaryE - Energy of primary
  h_crvinfomc_primaryE = new TH1F("h_crvinfomc_primaryE", "h_crvinfomc._primaryE", 100, 0, 200000);
  h_crvinfomc_primaryE->SetXTitle("Primary Particle Energy (MeV)");

  //Downstream Electron
  //deent.mom - Momentum of downstream electron
  if (makeCuts)
    h_deent_mom = new TH1F("h_deent_mom", "deent.mom", 25, 100, 110);
  else
    h_deent_mom = new TH1F("h_deent_mom", "deent.mom", 100, 0, 300);
  h_deent_mom->SetXTitle("Momentum (MeV/c)");

  //deent.do - Distance of downstream electron from the center of the detecto
  if (makeCuts)
    h_deent_d0 = new TH1F("h_deent_d0", "deent.d0", 100, -150, 150);
  else
    h_deent_d0 = new TH1F("h_deent_d0", "deent.d0", 100, -500, 500);
  h_deent_d0->SetXTitle("Distance to Center (mm)");

  //deent.td - Pitch angle of downstream electron
  if (makeCuts)
    h_deent_td = new TH1F("h_deent_td", "deent.td", 100, 0.5, 1.5);
  else
    h_deent_td = new TH1F("h_deent_td", "deent.td", 100, 0, 5);
  h_deent_td->SetXTitle("Pitch Angle");

  //deent.t0 - t_0 of downstream electron track
  if (makeCuts)
     h_de_t0 = new TH1F("h_de_t0", "de.t0", 100, 500, 1700);
  else
    h_de_t0 = new TH1F("h_de_t0", "de.t0", 100, 400, 1700);
  h_de_t0->SetXTitle("Time (ns)");

  //dequal.trkQualDeM - Track quality score of downstream electron track
  if (INC_TRKQUAL_SFX)
    {
      if (makeCuts)
	h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQualDeM", 100, 0.4, 1);
      else
	h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQualDeM", 100, 0, 1);
      h_dequal_trkQualDeM->SetXTitle("Track Quality");
    }
  else
    {
      if (makeCuts)
	h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQual", 100, 0.4, 1);
      else
	h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQual", 100, 0, 1);
      h_dequal_trkQualDeM->SetXTitle("Track Quality");
    }

  //demcent.d0 - MC truth downstream electron distance at tracker entrance
  if (makeCuts)
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -150, 150);
  else
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -550, 500);
  h_demcent_d0->SetXTitle("Distance from center (mm)");

  //Timing Comparisons
  //de.t0:crvinfo._timeWindowStart - t0 of tracker vs t0 of CRV
  h_det0_vs_CRVtimeWindowStart = new TH2F("h_det0_vs_CRVtimeWindowStart","de.t0:crvinfo._timeWindowStart", 100, 400, 1750, 100, 400, 1750);
  h_det0_vs_CRVtimeWindowStart->SetXTitle("t_0 at CRV (ns)");
  h_det0_vs_CRVtimeWindowStart->SetYTitle("t_0 at Tracker (ns)");
  h_det0_vs_CRVtimeWindowStart->SetStats(false);

  //de.t0:crvinfomc._time - MC Truth time at tracker vs CRV
  h_det0_vs_crvinfomc_time = new TH2F("h_det0_vs_crvinfomc_time","de.t0:crvinfomc._time", 100, -100, 1750, 100, 400, 1750);
  h_det0_vs_crvinfomc_time->SetXTitle("MC Truth t_0 at CRV (ns)");
  h_det0_vs_crvinfomc_time->SetYTitle("t_0 at Tracker (ns)");
  h_det0_vs_crvinfomc_time->SetStats(false);

  //de.t0 - crvinfo._timeWindowStart - Time delta of tracker to real CRV hits
  h_delta_det0_CRVtimeWindowStart = new TH1F("h_delta_det0_CRVtimeWindowStart", "de.t0 - crvinfo._timeWindowStart", 100, -300 , 300);
  h_delta_det0_CRVtimeWindowStart->SetXTitle("Time Delta (ns)");

  //de.t0 - crvinfomc._time - Time delta of tracker to MC Truth CRV of CRV hits
  h_delta_det0_crvinfomc_time = new TH1F("h_delta_det0_crvinfomc_time", "de.t0 - crvinfomc._time", 100, -800, 1000);
  h_delta_det0_crvinfomc_time->SetXTitle("Time Delta (ns)");

  //Otherplots
  //demcpri.momz / sqrt(demcpri.momx^2 + demcpri.momy^2 + demcpri.momz^2) - p_z/p
  h_pz_p = new TH1F("h_pz_p","p_z / p", 100, -1, 1);
  h_pz_p->SetXTitle("cos(#theta)");

  //PDG ID of events with no CRV coincidences
  h_noCRV_demc_pdg = new TH1F("h_noCRV_demc_pdg", "PDG ID of Particles Producing Events with no CRV Coincidences", 100, -300, 2300);
  h_noCRV_demc_pdg->SetXTitle("PDG ID");

  //Theta vs Tan Dip
  h_theta_vs_tandip = new TH2F("h_theta_vs_tandip", "arccos(p_z / p) : deent.td", 100, 0, 2.5, 100, 0, 180); //y limits -0.5-3.5
  h_theta_vs_tandip->SetXTitle("Tan Dip");
  h_theta_vs_tandip->SetYTitle("#theta (#circ)");
  h_theta_vs_tandip->SetStats(false);

  //Delta t vs z at crv
  h_delta_t_vs_z = new TH2F("h_delta_t_vs_z", "de.t0 - crvinfo._timeWindowStart : z", 100, -3, 19, 100, -100, 150);
  h_delta_t_vs_z->SetXTitle("MC Truth z at CRV (m)");
  h_delta_t_vs_z->SetYTitle("Time Delta (ns)");
  h_delta_t_vs_z->SetStats(false);

  //demcgen.t0
  h_demcgen_t0 = new TH1F("h_demcgen_t0", "demcgen.t0", 100, 400, 900);
  h_demcgen_t0->SetXTitle("t_0 (ns)");

  //demcpri.t0
  h_demcpri_t0 = new TH1F("h_demcpri_t0", "demcpri.t0", 100, 400, 900);
  h_demcpri_t0->SetXTitle("t_0 (ns)");

  //h_delta_t_minus_demcpri_t0
  h_delta_t_minus_demcpri_t0 = new TH1F("h_delta_t_minus_demcpri_t0", "de.t0 - crvinfo._timeWindowStart -demcpri.t0", 100, -500, 300);
  h_delta_t_minus_demcpri_t0->SetXTitle("(ns)");


  //h_de_t0_minus_demcpri_t0
  h_de_t0_minus_demcpri_t0 = new TH1F("h_de_t0_minus_demcpri_t0", "de.t0 - demcpri.t0", 100, 0, 500);
  h_de_t0_minus_demcpri_t0->SetXTitle("(ns)");

  //Neutron kinetic energy
  h_neutronKE = new TH1F("h_neutronKE", "Kinetic Energy of Neutron Primaries", 100, 0, 350000);
  h_neutronKE->SetXTitle("(MeV)");

  //Neutron kinetic energy
  h_neutronKE_noCRV = new TH1F("h_neutronKE_noCRV", "Kinetic Energy of Neutron Primaries without CRV Hits", 50, 0, 25000);
  h_neutronKE_noCRV->SetXTitle("(MeV)");

  
  if (INC_CRV_SUMMARIES)
    {
      //MC Truth: Energy deposited in the CRV
      h_crvsummarymc_total_E_dep = new TH1F("h_crvsummarymc_total_E_dep", "MC Truth: Total E Deposited in CRV", 100, 0, 100);
      h_crvsummarymc_total_E_dep->SetXTitle("Energy (MeV");

      //MC Truth: Number of Counters hit in the CRV
      h_nHitCounters = new TH1F("h_nHitCounters", "Number of Counters Hit in the CRV", 20,-0.5, 19.5);
      h_nHitCounters->SetXTitle("Number of Counters");

      /*
      h_crvinfomcplane_primaryZ_vs_X = new TH2F("h_crvinfomcplane_primaryZ_vs_X", "MC Truth: Location where Primary Hit CRV", 100,-100, 60, 100,-100, 120); 
      h_crvinfomcplane_primaryZ_vs_X->SetXTitle("z (m)");
      h_crvinfomcplane_primaryZ_vs_X->SetYTitle("x (m)");
      h_crvinfomcplane_primaryZ_vs_X->SetStats(false);*/
      
    }

}


/*********************************************************************************************************************************************************/
/////////////////////////////////////////////////////    Clean Up Histograms & Graphs   ///////////////////////////////////////////////////////////////////

void deleteHists()
{
  h_crvinfomc_x0->Delete();
  h_crvinfomc_y0->Delete();
  h_crvinfomc_z0->Delete();
  h_crvinfomc_z0_vs_x0->Delete();
  h_crvinfomc_z0_vs_y0->Delete();
  h_crvinfomc_primaryX->Delete();
  h_crvinfomc_primaryY->Delete();
  h_crvinfomc_primaryZ->Delete();
  h_crvinfomc_primaryZ_vs_X->Delete();
  h_crvinfomc_primaryPdgId->Delete();
  h_crvinfomc_primaryE->Delete();
  h_deent_mom->Delete();
  h_deent_d0->Delete();
  h_deent_td->Delete();
  h_de_t0->Delete();
  h_dequal_trkQualDeM->Delete();
  h_demcent_d0->Delete();
  h_det0_vs_CRVtimeWindowStart->Delete();
  h_det0_vs_crvinfomc_time->Delete();
  h_delta_det0_CRVtimeWindowStart->Delete();
  h_delta_det0_crvinfomc_time->Delete();
  h_pz_p->Delete();
  h_noCRV_demc_pdg->Delete();
  h_delta_t_vs_z->Delete();
  h_demcgen_t0->Delete();
  h_demcpri_t0->Delete();
  h_de_t0_minus_demcpri_t0->Delete();
  h_neutronKE->Delete();
  h_neutronKE_noCRV->Delete();
  h_crvsummarymc_total_E_dep->Delete();
  h_nHitCounters->Delete();
  //h_crvinfomcplane_primaryZ_vs_X->Delete();


  // g_crvinfomc_z0_vs_x0->Delete(); 
  // g_crvinfomc_z0_vs_y0->Delete();
  // g_crvinfomc_primaryZ_vs_X->Delete();
  // g_det0_vs_CRVtimeWindowStart->Delete();
  // g_det0_vs_crvinfomc_time->Delete();
  // g_oposx_vs_oposz->Delete();
  //  g_oposy_vs_oposz->Delete();

}


/*********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Make Standardized Plots    /////////////////////////////////////////////////////////////////////////

/* Produce and save plots of each histogram
   @param treePath : Path to the TrkAna tree to be plotted
   @param neg : true if plotting the negative tree, false for the positive tree
   @param makeCuts : true if cuts should be applied
   @param momCut : 0 for no cuts, 1 for physics window cuts, 2 for scaled window cuts
   @param onlyScan : If true, doesn't make plots - only performs initial events counts and scans
 */
void makeStandardizedPlots(string treePath, bool neg, bool makeCuts, int momCut = 2, bool onlyScan = false)
{

  /********************************************************************************************************************************************************/
  //////////////////////////////////////////////////    Define Standard Cuts    ///////////////////////////////////////////////////////////////////////////

  //Signal cuts
  string momentum_cut = "deent.mom>100 && deent.mom<110"; //Use wider momentum window than experimental signal window
  string signal_mom_cut = "deent.mom>103.75 && deent.mom<105";

  string no_upstream = "ue.status<0";
  string trk_qual;
  string trk_cut_pid;
  if (INC_TRKQUAL_SFX)
    {
      trk_qual = "dequal.TrkQualDeM>0.8";; //For original CRY1 analysis this was 0.4
      trk_cut_pid = "dequal.TrkPIDDeM>0.9";
    }
  else
    {
      trk_qual = "dequal.TrkQual>0.8";; //For original CRY1 analysis this was 0.4
      trk_cut_pid = "dequal.TrkPID>0.9";
    }
  string pitch_angle = "deent.td>0.57735027 && deent.td<1"; //  Excludes beam particles
  string min_trans_R = "deent.d0>-80 && deent.d0<105"; //  Consistent with coming from the target
  string max_trans_R = "(deent.d0+2.0/deent.om)>450. && (deent.d0+2.0/deent.om)<680."; //  Inconsistent with hitting the proton absorber
  string timing_cut = "de.t0>700 && de.t0<1695"; // This is the standard window
  string all_cuts_MDC = momentum_cut + "&&" + no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
  string signalCuts = all_cuts_MDC + "&&" + trk_cut_pid + "&&" + timing_cut;
  string noMom = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid + "&&" + timing_cut; 
  string physicsCuts = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid + "&&" + timing_cut + "&&" + signal_mom_cut;
  
  //Alternative cuts
  string d0is0 = "demcent.d0==0";
  string noMomNoPID = trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
  string testCut = momentum_cut + "&&" + no_upstream + "&&" + trk_qual + "&&" + trk_cut_pid + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + timing_cut; 
  string testCut2 = noMom;
  


  //////////////////////////////////////////////////////////// e^+ cuts //////////////////////////////////////////////////////////////////////////////////
  string nactive = "de.nactive > 20 && de.nactive < 200";
  string nhits_minus_nactive = "(de.nhits - de.nactive) > 0 && (de.hits - de.nactive) < 5";
  string perr = "deent.momerr > 0 && deent.momerr < 0.4";
  string t0err = "de.t0err> 0 && de.t0err < 1.5";
  string tandip = "deent.td > 0.57";
  string d0 = "-1*deent.d0 > -100 && -1*deent < 100";
  string rmax = "abs(deent.d0+2.0/deent.om)>430. && abs(deent.d0+2.0/deent.om)<690.";
  string chisqrd_dof = "(de.chisq / de.ndof) > 0 && (de.chisq / de.ndof) < 5";
  string mom = "deent.mom > 90.5 && deent.mom < 92.5";
  string t0 = "de.t0 > 700 && de.t0 < 1695";
  string ePlusCuts =nactive+"&&"+nhits_minus_nactive+"&&"+perr+"&&"+t0err+"&&"+tandip+"&&"+d0+"&&"+rmax+"&&"+chisqrd_dof+"&&"+mom+"&&"+t0; //std cuts
  // string ePlusCuts =nactive+"&&"+nhits_minus_nactive+"&&"+perr+"&&"+t0err+"&&"+tandip+"&&"+d0+"&&"+chisqrd_dof+"&&"+mom+"&&"+t0+"&&"+trk_cut_pid+"&&"+trk_qual; //Add pid + trk qual
  string ePlus_noMom = nactive+"&&"+nhits_minus_nactive+"&&"+perr+"&&"+t0err+"&&"+tandip+"&&"+d0+"&&"+rmax+"&&"+chisqrd_dof+"&&"+t0;
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //Read tree from file
  TFile inFile(treePath.c_str());
  TTree *tree;
  if (neg) 
    tree = (TTree*) inFile.Get("TrkAnaNeg/trkana");
  else
    tree = (TTree*) inFile.Get("TrkAnaPos/trkana");
  
  if (tree == NULL)
    {
      cout << "Tree cannot be found/read. Check the filepath" << endl;
      exit(2);
    }
  
  //Choose which cuts to use
  TCut cuts;
  string cutIdentifier;
  if (makeCuts)
    {     
      if (momCut == 2)
	cuts = TCut(signalCuts.c_str());
      else if (momCut == 1)
	cuts =TCut(physicsCuts.c_str());
      else
	cuts = TCut(noMom.c_str());

      if (!neg)
	{
	  if (momCut == 1)
	    cuts = TCut(ePlusCuts.c_str());
	  else
	    cuts = TCut(ePlus_noMom.c_str());
	}
	
     
      
      cutIdentifier = "_cut";
    }
  else
    {
      cuts = "";
      cutIdentifier = "";
    }

  
  //Print out some information about the PDG of events 
  if (false) //////////////////////////////////////////////////// Change this if you want the below scans to run
    {
      cout << "Events which were not produced by a muon that did not produce coincidences in the CRV" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demcgen.pdg",cuts + "abs(demcgen.pdg)!=13" + "@crvinfo.size()<1","");
      cout << "\n" << endl;

      cout << "Non-muon/electron background events" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "abs(demc.pdg)>211","");

      cout << "mu- background events" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "(demc.pdg)==13","");
  
      cout << "mu+ background events" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "demc.pdg==-13","");
  
      cout << "e+ background events" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "(demc.pdg)==-11","");

      cout <<"\n" << endl;
    }
  else 
    {
      cout << "\nReconstructed particles (demc.pdg)" << endl;
      cout << "Number of e- events = " << tree->GetEntries(cuts + "demc.pdg==11")  << endl;
      cout << "Number of e+ events = " << tree->GetEntries(cuts + "demc.pdg==-11")  << endl;
      cout << "Number of mu- events = " << tree->GetEntries(cuts + "demc.pdg==13")  << endl;
      cout << "Number of mu+ events = " << tree->GetEntries(cuts + "demc.pdg==-13")  << endl;

      cout << "\nPrimary particles (demcgen.pdg)" << endl;
      cout << "Number of n events = " << tree->GetEntries(cuts + "demcgen.pdg==2112")  << endl;
      cout << "Number of p+ events = " << tree->GetEntries(cuts + "demcgen.pdg==2212")  << endl;
      cout << "Number of mu- events = " << tree->GetEntries(cuts + "demcgen.pdg==13")  << endl;
      cout << "Number of mu+ events = " << tree->GetEntries(cuts + "demcgen.pdg==-13")  << endl;
      cout << "Number of pi- events = " << tree->GetEntries(cuts + "demcgen.pdg==-211")  << endl;
      cout << "Number of pi+ events = " << tree->GetEntries(cuts + "demcgen.pdg==211")  << endl;

      cout << "Number of events without CRV coincidences = " << tree->GetEntries(cuts + "@crvinfo.size()<1") << endl;
      
     
      cout << "\nOther reconstructed events:" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "abs(demc.pdg)>211","");
      cout << "Events which were not produced by a muon that did not produce coincidences in the CRV:" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demcgen.pdg:demc.pdg:deent.mom",cuts +  "abs(demcgen.pdg)!=13" + "@crvinfo.size()<1","");
      cout << "\n" << endl;  

      cout << "Muon events without CRV coincidences:" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demcgen.pdg:demc.pdg:deent.mom",cuts +  "abs(demcgen.pdg)=13" + "@crvinfo.size()<1","");

      cout << "\nPrimary particles (demcgen.pdg)" << endl;
      double totalEvs = tree->GetEntries(cuts+"@crvinfo.size()<1");
      cout << "Fraction of unvetoed events from neutrons = " << tree->GetEntries(cuts + "demcgen.pdg==2112" +"@crvinfo.size()<1") / totalEvs  << endl;
      cout << "Fraction of unvetoed events from protons = " << tree->GetEntries(cuts + "demcgen.pdg==2212" +"@crvinfo.size()<1") / totalEvs  << endl;
      cout << "Fraction of unvetoed events from mu+s = " << tree->GetEntries(cuts + "demcgen.pdg==13"+"@crvinfo.size()<1") /totalEvs  << endl;
      cout << "Fraction of unvetoed events from mu-s = " << tree->GetEntries(cuts + "demcgen.pdg==-13"+"@crvinfo.size()<1") /totalEvs  << endl;
      cout << "Fraction of unvetoed events from pi-s =" << tree->GetEntries(cuts + "demcgen.pdg==-211"+"@crvinfo.size()<1") / totalEvs  << endl;
      cout << "Number of pi+ events = " << tree->GetEntries(cuts + "demcgen.pdg==211"+"@crvinfo.size()<1") /totalEvs << endl;
     
    }

  if (makeCuts)
    {
      cout << "Non-electron reconstructed events: " << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg",cuts + "demc.pdg!=11","");
      
    }

  /////// Other scans
  // cout << "Events in the delta t tail:" << endl;
  // tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg:(de.t0 - crvinfo._timeWindowStart):ue.nHits",cuts + "(de.t0 - crvinfo._timeWindowStart)>40","");
  // cout << "Events with t0 < 800 ns:" << endl;
  // tree->Scan("evtinfo.subrunid:evtinfo.eventid:demc.pdg:de.t0",cuts + "de.t0<800","");
  // cout << "Events in lower cluster of delta t vs z plot" << endl;
  //tree->Scan("evtinfo.subrunid:evtinfo.eventid:(de.t0 - crvinfo._timeWindowStart):crvinfomc._z[0]:demc.pdg:demcgen.pdg",cuts +"(de.t0 - crvinfo._timeWindowStart)<0&&crvinfomc._z[0]/1000>12","");
  // cout << "Events in upper cluster of delta t vs z plot" << endl;
  //tree->Scan("evtinfo.subrunid:evtinfo.eventid:(de.t0 - crvinfo._timeWindowStart):crvinfomc._z[0]:demc.pdg:demcgen.pdg",cuts + "(de.t0 - crvinfo._timeWindowStart)>30&&crvinfomc._z[0]/1000>12","");

  if (onlyScan) 
    exit(0);

  //Initialize the histograms
  initializeHists(makeCuts);

  //Set up canvases
  TCanvas *canv = new TCanvas("canv", "TrkAna Plots", 800, 600);
  TCanvas *logCanv = new TCanvas("logCanv", "TrkAna Plots - Logscale", 800, 600);
  logCanv->SetLogy(1);
  
  //Choose which stats to displace
  //gStyle->SetOptStat(111110);
  gStyle->SetOptStat(1111111);

  //Choose file format to use
  string filetype = ".png";
  //string filetype = ".pdf";


  //Fill and plot and save histograms

  //crvinfomc_x0
  canv->cd();
  tree->Draw("crvinfomc._x[0]/1000>>h_crvinfomc_x0",cuts, "");
  h_crvinfomc_x0 = (TH1F*) gDirectory->Get("h_crvinfomc_x0");
  h_crvinfomc_x0->Draw("hist");
  canv->SaveAs(("../Plots/crvinfomc_x0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_x0->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_x0_logY" + cutIdentifier + filetype).c_str());
  
  //crvinfomc_y0
  tree->Draw("crvinfomc._y[0]/1000>>+h_crvinfomc_y0",cuts, "goff");
  h_crvinfomc_y0 = (TH1F*) gDirectory->Get("h_crvinfomc_y0");
  canv->cd();
  h_crvinfomc_y0->Draw();
  canv->SaveAs(("../Plots/crvinfomc_y0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_y0->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_y0_logY" + cutIdentifier + filetype).c_str());

  //crvinfomc_z0
  tree->Draw("crvinfomc._z[0]/1000>>+h_crvinfomc_z0",cuts, "goff");
  h_crvinfomc_z0 = (TH1F*) gDirectory->Get("h_crvinfomc_z0");
  canv->cd();
  h_crvinfomc_z0->Draw();
  canv->SaveAs(("../Plots/crvinfomc_z0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_z0->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_z0_logY" + cutIdentifier + filetype).c_str());

  //h_crvinfomc_z0_vs_x0
  tree->Draw("crvinfomc._x[0]/1000:crvinfomc._z[0]/1000>>+h_crvinfomc_z0_vs_x0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0_vs_x0->Draw("colz");
  canv->SaveAs(("../Plots/crvinfomc_z0_vs_x0" + cutIdentifier + filetype).c_str());

  canv->cd();
  tree->Draw("crvinfomc._x[0]/1000:crvinfomc._z[0]/1000",cuts);
  g_crvinfomc_z0_vs_x0 = (TGraph*) gPad->GetPrimitive("Graph");
  g_crvinfomc_z0_vs_x0->SetTitle("crvinfomc._z[0]:crvinfomc._x[0];z at CRV (m); x at CRV (m)");
  canv->Update();
  canv->SaveAs(("../Plots/crvinfomc_z0_vs_x0_graph" + cutIdentifier + filetype).c_str());

  //h_crvinfomc_z0_vs_y0
  tree->Draw("crvinfomc._y[0]/1000:crvinfomc._z[0]/1000>>+h_crvinfomc_z0_vs_y0",cuts);
  canv->cd();
  h_crvinfomc_z0_vs_y0->Draw("colz");
  canv->SaveAs(("../Plots/crvinfomc_z0_vs_y0" + cutIdentifier + filetype).c_str());

  canv->cd();
  tree->Draw("crvinfomc._y[0]/1000:crvinfomc._z[0]/1000",cuts);
  g_crvinfomc_z0_vs_y0 = (TGraph*) gPad->GetPrimitive("Graph");
  g_crvinfomc_z0_vs_y0->SetTitle("crvinfomc._z[0]:crvinfomc._y[0];z at CRV (m); y at CRV (m)");
  canv->SaveAs(("../Plots/crvinfomc_z0_vs_y0_graph" + cutIdentifier + filetype).c_str());
  
  //crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX/1000>>+h_crvinfomc_primaryX",cuts, "goff");
  h_crvinfomc_primaryX = (TH1F*) gDirectory->Get("h_crvinfomc_primaryX");
  canv->cd();
  h_crvinfomc_primaryX->SetTitle("crvinfomc._primaryX");
  h_crvinfomc_primaryX->SetXTitle("Primary Particle x Pos at Gen Plane (m)");
  h_crvinfomc_primaryX->Draw();
  canv->SaveAs(("../Plots/crvinfomc_primaryX" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_primaryX->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_primaryX_logY" + cutIdentifier + filetype).c_str());

  //crvinfomc._primaryY
  tree->Draw("crvinfomc._primaryY/1000>>+h_crvinfomc_primaryY",cuts, "goff");
  h_crvinfomc_primaryY = (TH1F*) gDirectory->Get("h_crvinfomc_primaryY");
  h_crvinfomc_primaryY->SetTitle("crvinfomc._primaryY");
  h_crvinfomc_primaryY->SetXTitle("Primary Particle y Pos at Gen Plane (m)");
  canv->cd();
  h_crvinfomc_primaryY->Draw();
  canv->SaveAs(("../Plots/crvinfomc_primaryY" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_primaryY->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_primaryY_logY" + cutIdentifier + filetype).c_str());

  //crvinfomc._primaryZ
  tree->Draw("crvinfomc._primaryZ/1000>>+h_crvinfomc_primaryZ",cuts, "goff");
  h_crvinfomc_primaryZ = (TH1F*) gDirectory->Get("h_crvinfomc_primaryZ");
  h_crvinfomc_primaryZ->SetTitle("crvinfomc._primaryZ");
  h_crvinfomc_primaryZ->SetXTitle("Primary Particle z Pos at Gen Plane (m)");
  
  canv->cd();
  h_crvinfomc_primaryZ->Draw();
  canv->SaveAs(("../Plots/crvinfomc_primaryZ" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_primaryZ->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_primaryZ_logY" + cutIdentifier + filetype).c_str());

  //cvinfomc._primaryZ:crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX/1000:crvinfomc._primaryZ/1000>>+h_crvinfomc_primaryZ_vs_X",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryZ_vs_X->Draw("colz");
  canv->SaveAs(("../Plots/crvinfomc_primaryZ_vs_X" + cutIdentifier + filetype).c_str());

  canv->cd();
  tree->Draw("crvinfomc._primaryX:crvinfomc._primaryZ",cuts);
  g_crvinfomc_primaryZ_vs_X = (TGraph*) gPad->GetPrimitive("Graph");
  g_crvinfomc_primaryZ_vs_X->SetTitle("crvinfomc._primaryZ:crvinfomc._primaryX;z (m); x (m)");
  canv->Update();
  canv->SaveAs(("../Plots/crvinfomc_primaryZ_vs_X_graph" + cutIdentifier + filetype).c_str());

  //crvinfomc._primaryPdgId
  tree->Draw("crvinfomc._primaryPdgId>>+h_crvinfomc_primaryPdgId",cuts, "goff");
  h_crvinfomc_primaryPdgId = (TH1F*) gDirectory->Get("h_crvinfomc_primaryPdgId");
  canv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  canv->SaveAs(("../Plots/crvinfomc_primaryPdgId" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_primaryPdgId_logY" + cutIdentifier + filetype).c_str());
  
  //crvinfomc._primaryE
  tree->Draw("crvinfomc._primaryE>>+h_crvinfomc_primaryE",cuts, "goff");
  h_crvinfomc_primaryE = (TH1F*) gDirectory->Get("h_crvinfomc_primaryE");
  canv->cd();
  h_crvinfomc_primaryE->Draw();
  canv->SaveAs(("../Plots/crvinfomc_primaryE" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_crvinfomc_primaryE->Draw();
  logCanv->SaveAs(("../Plots/crvinfomc_primaryE_logY" + cutIdentifier + filetype).c_str());

  //deent.mom
  tree->Draw("deent.mom>>+h_deent_mom",cuts, "goff");
  h_deent_mom = (TH1F*) gDirectory->Get("h_deent_mom");
  canv->cd();
  h_deent_mom->Draw();
  canv->SaveAs(("../Plots/deent_mom" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_mom->Draw();
  logCanv->SaveAs(("../Plots/deent_mom_logY" + cutIdentifier + filetype).c_str());

  //deent.d0
  tree->Draw("deent.d0>>+h_deent_d0",cuts, "goff");
  h_deent_d0 = (TH1F*) gDirectory->Get("h_deent_d0");
  canv->cd();
  h_deent_d0->Draw();
  canv->SaveAs(("../Plots/deent_d0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_d0->Draw();
  logCanv->SaveAs(("../Plots/deent_d0_logY" + cutIdentifier + filetype).c_str());

  //deent.td
  tree->Draw("deent.td>>+h_deent_td",cuts, "goff");
  h_deent_td = (TH1F*) gDirectory->Get("h_deent_td");
  canv->cd();
  h_deent_td->Draw();
  canv->SaveAs(("../Plots/deent_td" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_td->Draw();
  logCanv->SaveAs(("../Plots/deent_td_logY" + cutIdentifier + filetype).c_str());

  //de.t0
  tree->Draw("de.t0>>+h_de_t0",cuts, "goff");
  h_de_t0 = (TH1F*) gDirectory->Get("h_de_t0");
  canv->cd();
  h_de_t0->Draw();
  canv->SaveAs(("../Plots/de_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_de_t0->Draw();
  logCanv->SaveAs(("../Plots/de_t0_logY" + cutIdentifier + filetype).c_str());

  //dequal.TrkQualDeM or dequal.TrkQual
  if (INC_TRKQUAL_SFX)
    {
      tree->Draw("dequal.TrkQualDeM>>+h_dequal_trkQualDeM",cuts, "goff");
      h_dequal_trkQualDeM = (TH1F*) gDirectory->Get("h_dequal_trkQualDeM");
      canv->cd();
      h_dequal_trkQualDeM->Draw();
      canv->SaveAs(("../Plots/dequal_trkQualDeM" + cutIdentifier + filetype).c_str());
      logCanv->cd();
      h_dequal_trkQualDeM->Draw();
      logCanv->SaveAs(("../Plots/dequal_trkQualDeM_logY" + cutIdentifier + filetype).c_str());
    }
  else
    {
      tree->Draw("dequal.TrkQual>>+h_dequal_trkQualDeM",cuts, "goff");
      h_dequal_trkQualDeM = (TH1F*) gDirectory->Get("h_dequal_trkQualDeM");
      canv->cd();
      h_dequal_trkQualDeM->Draw();
      canv->SaveAs(("../Plots/dequal_trkQual" + cutIdentifier + filetype).c_str());
      logCanv->cd();
      h_dequal_trkQualDeM->Draw();
      logCanv->SaveAs(("../Plots/dequal_trkQual_logY" + cutIdentifier + filetype).c_str());
    }

  //demcent.d0
  tree->Draw("demcent.d0>>+h_demcent_d0",cuts, "goff");
  h_demcent_d0 = (TH1F*) gDirectory->Get("h_demcent_d0");
  canv->cd();
  h_demcent_d0->Draw();
  canv->SaveAs(("../Plots/demcent_d0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_demcent_d0->Draw();
  logCanv->SaveAs(("../Plots/demcent_d0_logY" + cutIdentifier + filetype).c_str());

  //de.t0:crvinfo._timeWindowStart
  tree->Draw("de.t0:crvinfo._timeWindowStart>>+h_det0_vs_CRVtimeWindowStart",cuts, "goff");
  canv->cd();
  h_det0_vs_CRVtimeWindowStart->Draw("colz");
  canv->SaveAs(("../Plots/det0_vs_CRVtimeWindowStart" + cutIdentifier + filetype).c_str());

  canv->cd();
  tree->Draw("de.t0:crvinfo._timeWindowStart",cuts);
  g_det0_vs_CRVtimeWindowStart = (TGraph*) gPad->GetPrimitive("Graph");
  g_det0_vs_CRVtimeWindowStart->SetTitle("de.t0:crvinfo._timeWindowStart;crvinfo._timeWindowStart; de.t0(m)");
  canv->Update();
  canv->SaveAs(("../Plots/det0_vs_CRVtimeWindowStart_graph" + cutIdentifier + filetype).c_str());
  
  //de.t0:crvinfomc._time
  tree->Draw("de.t0:crvinfomc._time>>+h_det0_vs_crvinfomc_time",cuts, "goff");
  canv->cd();
  h_det0_vs_crvinfomc_time->Draw("colz");
  canv->SaveAs(("../Plots/det0_vs_crvinfomc_time" + cutIdentifier + filetype).c_str());

  canv->cd();
  tree->Draw("de.t0:crvinfomc._time",cuts);
  g_det0_vs_crvinfomc_time = (TGraph*) gPad->GetPrimitive("Graph");
  g_det0_vs_crvinfomc_time->SetTitle("de.t0:crvinfo._crvinfomc_time;crvinfomc._time; de.t0(m)");
  canv->Update();
  canv->SaveAs(("../Plots/det0_vs_crvinfomc_time_graph" + cutIdentifier + filetype).c_str());

  //de.t0 - crvinfo._timeWindowStart
  tree->Draw("(de.t0 - crvinfo._timeWindowStart)>>+h_delta_det0_CRVtimeWindowStart",cuts, "goff");
  h_delta_det0_CRVtimeWindowStart = (TH1F*) gDirectory->Get("h_delta_det0_CRVtimeWindowStart");
  canv->cd();
  h_delta_det0_CRVtimeWindowStart->Draw();
  canv->SaveAs(("../Plots/delta_det0_CRVtimeWindowStart" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_delta_det0_CRVtimeWindowStart->Draw();
  logCanv->SaveAs(("../Plots/delta_det0_CRVtimeWindowStart_logY" + cutIdentifier + filetype).c_str());

  //de.t0 - crvinfomc._time
  tree->Draw("(de.t0 - crvinfomc._time)>>+h_delta_det0_crvinfomc_time",cuts, "goff");
  h_delta_det0_crvinfomc_time = (TH1F*) gDirectory->Get("h_delta_det0_crvinfomc_time");
  canv->cd();
  h_delta_det0_crvinfomc_time->Draw();
  canv->SaveAs(("../Plots/delta_det0_crvinfomc_time" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_delta_det0_crvinfomc_time->Draw();
  logCanv->SaveAs(("../Plots/delta_det0_crvinfomc_time_logY" + cutIdentifier + filetype).c_str());

  //cos(theta) = p_z / p
  tree->Draw("demcpri.momz / sqrt((demcpri.momx * demcpri.momx)+(demcpri.momy * demcpri.momy)+(demcpri.momz * demcpri.momz)) >>+h_pz_p",cuts, "goff");
  h_pz_p = (TH1F*) gDirectory->Get("h_pz_p");
  canv->cd();
  h_pz_p->Draw();
  canv->SaveAs(("../Plots/cos_theta" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_pz_p->Draw();
  logCanv->SaveAs(("../Plots/cos_theta_logY" + cutIdentifier + filetype).c_str());

  //demc.pdg of events with no CRV coincidences
  TCut noCRVcoincidences = "@crvinfo.size()<1";
  tree->Draw("demc.pdg>>+h_noCRV_demc_pdg",cuts + noCRVcoincidences, "goff");
  h_noCRV_demc_pdg = (TH1F*) gDirectory->Get("h_noCRV_demc_pdg");
  canv->cd();
  h_noCRV_demc_pdg->Draw();
  canv->SaveAs(("../Plots/noCRV_demc_pdg" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_noCRV_demc_pdg->Draw();
  logCanv->SaveAs(("../Plots/noCRV_demc_pdg_logY" + cutIdentifier + filetype).c_str());

  
  //cos(theta) = p_z / p
  tree->Draw("360.0/6.2832*acos(demcpri.momz / sqrt((demcpri.momx * demcpri.momx)+(demcpri.momy * demcpri.momy)+(demcpri.momz * demcpri.momz))):deent.td >>+h_theta_vs_tandip", cuts, "goff");
  h_theta_vs_tandip = (TH2F*) gDirectory->Get("h_theta_vs_tandip");
  canv->cd();
  h_theta_vs_tandip->Draw("colz");
  canv->SaveAs(("../Plots/theta_vs_tandip" + cutIdentifier + filetype).c_str());


  // h_delta_t_vs_z
  tree->Draw("(de.t0 - crvinfo._timeWindowStart):crvinfomc._z[0]/1000 >>+h_delta_t_vs_z", cuts, "goff");
  h_delta_t_vs_z = (TH2F*) gDirectory->Get("h_delta_t_vs_z");
  canv->cd();
  h_delta_t_vs_z->Draw("colz");
  canv->SaveAs(("../Plots/deltat_vs_z" + cutIdentifier + filetype).c_str());


  //demcgen.t0
  tree->Draw("demcgen.t0>>+h_demcgen_t0",cuts, "goff");
  h_demcgen_t0 = (TH1F*) gDirectory->Get("h_demcgen_t0");
  canv->cd();
  h_demcgen_t0->Draw();
  canv->SaveAs(("../Plots/demcgen_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_demcgen_t0->Draw();
  logCanv->SaveAs(("../Plots/demcgen_t0_logY" + cutIdentifier + filetype).c_str());

  //demcpri.t0
  tree->Draw("demcpri.t0>>+h_demcpri_t0",cuts, "goff");
  h_demcpri_t0 = (TH1F*) gDirectory->Get("h_demcpri_t0");
  canv->cd();
  h_demcpri_t0->Draw();
  canv->SaveAs(("../Plots/demcpri_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_demcpri_t0->Draw();
  logCanv->SaveAs(("../Plots/demcpri_t0_logY" + cutIdentifier + filetype).c_str());
  
 
  //h_delta_t_minus_demcpri_t0
  tree->Draw("de.t0 - crvinfo._timeWindowStart - demcpri.t0>>+h_delta_t_minus_demcpri_t0",cuts, "goff");
  h_delta_t_minus_demcpri_t0 = (TH1F*) gDirectory->Get("h_delta_t_minus_demcpri_t0");
  canv->cd();
  h_delta_t_minus_demcpri_t0->Draw();
  canv->SaveAs(("../Plots/delta_t_minus_demcpri_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_delta_t_minus_demcpri_t0->Draw();
  logCanv->SaveAs(("../Plots/delta_t_minus_demcpri_t0_logy" + cutIdentifier + filetype).c_str());
  

  // h_de_t0_minus_demcpri_t0
  tree->Draw("de.t0 - demcpri.t0>>+h_de_t0_minus_demcpri_t0",cuts, "goff");
  h_de_t0_minus_demcpri_t0 = (TH1F*) gDirectory->Get("h_de_t0_minus_demcpri_t0");
  canv->cd();
  h_de_t0_minus_demcpri_t0->Draw();
  canv->SaveAs(("../Plots/de_t0_minus_demcpri_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_de_t0_minus_demcpri_t0->Draw();
  logCanv->SaveAs(("../Plots/de_t0_minus_demcpri_t0_logy" + cutIdentifier + filetype).c_str());
  

  //Neutron kinetic energy
  tree->Draw("sqrt(((demcgen.momx*demcgen.momx)+(demcgen.momy*demcgen.momy)+(demcgen.momz*demcgen.momz)) + (939.56*939.56)) - 939.56 >>+h_neutronKE",cuts+"demcgen.pdg==2112", "goff");
  h_neutronKE = (TH1F*) gDirectory->Get("h_neutronKE");
  canv->cd();
  h_neutronKE->Draw();
  canv->SaveAs(("../Plots/neutronKE" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_neutronKE->Draw();
  logCanv->SaveAs(("../Plots/neutronKE_logY" + cutIdentifier + filetype).c_str());

  //Neutron kinetic energy
  tree->Draw("sqrt(((demcgen.momx*demcgen.momx)+(demcgen.momy*demcgen.momy)+(demcgen.momz*demcgen.momz)) + (939.56*939.56)) - 939.56 >>+h_neutronKE_noCRV",cuts+"demcgen.pdg==2112"+"@crvinfo.size()<1", "goff");
  h_neutronKE_noCRV = (TH1F*) gDirectory->Get("h_neutronKE_noCRV");
  canv->cd();
  h_neutronKE_noCRV->Draw();
  canv->SaveAs(("../Plots/neutronKE_noCRV" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_neutronKE_noCRV->Draw();
  logCanv->SaveAs(("../Plots/neutronKE_noCRV_logY" + cutIdentifier + filetype).c_str());


  //x:z position of the electron/positron
  string pdgSign;
  if (neg)
    pdgSign = "";
  else
    pdgSign = "-";

  canv->cd();
  tree->Draw("demc.oposx:demc.oposz",("demc.pdg=="+pdgSign+"11").c_str());
  g_oposx_vs_oposz = (TGraph*) gPad->GetPrimitive("Graph");
  g_oposx_vs_oposz->SetTitle("Reconstructed e^- : Origin Position;z position (mm);x position (mm)");
  canv->Update();
  canv->SaveAs(("../Plots/demc_oposx_vs_oposz" + cutIdentifier + filetype).c_str());

  
  //y:z position of the electron/positron
  canv->cd();
  tree->Draw("demc.oposy:demc.oposz",("demc.pdg=="+pdgSign+"11").c_str());
  g_oposy_vs_oposz = (TGraph*) gPad->GetPrimitive("Graph");
  g_oposy_vs_oposz->SetTitle("Reconstructed e^- : Origin Position;z position (mm);y position (mm)");
  canv->Update();
  canv->SaveAs(("../Plots/demc_oposy_vs_oposz" + cutIdentifier + filetype).c_str());
  
  //y:x position of the electron/positron
  canv->cd();
  tree->Draw("demc.oposy:demc.oposx",("demc.pdg=="+pdgSign+"11").c_str());
  g_oposy_vs_oposx = (TGraph*) gPad->GetPrimitive("Graph");
  g_oposy_vs_oposx->SetTitle("Reconstructed e^- : Origin Position;x position (mm);y position (mm)");
  canv->Update();
  canv->SaveAs(("../Plots/demc_oposy_vs_oposx" + cutIdentifier + filetype).c_str());


  if (INC_CRV_SUMMARIES)
    {
      // MC Truth, total energy deposited in the CRV 
      tree->Draw("crvsummarymc._totalEnergyDeposited>>+h_crvsummarymc_total_E_dep",cuts, "goff");
      h_crvsummarymc_total_E_dep = (TH1F*) gDirectory->Get("h_crvsummarymc_total_E_dep");
      canv->cd();
      h_crvsummarymc_total_E_dep->Draw();
      canv->SaveAs(("../Plots/total_E_dep" + cutIdentifier + filetype).c_str());


      // MC Truth, number of counters hit xin the CRV 
      tree->Draw("crvsummarymc._nHitCounters>>+h_nHitCounters",cuts, "goff");
      h_nHitCounters = (TH1F*) gDirectory->Get("h_nHitCounters");
      canv->cd();
      h_nHitCounters->Draw();
      canv->SaveAs(("../Plots/nHitCounters" + cutIdentifier + filetype).c_str());


      //MC Truth location where primary reached the CRV
      canv->cd();
      tree->Draw("crvinfomcplane._primaryZ:crvinfomcplane._primaryX", cuts);
      g_crvinfomcplane_primaryZ_vs_X = (TGraph*) gPad->GetPrimitive("Graph");
      g_crvinfomcplane_primaryZ_vs_X->SetTitle("MCTruth Position where Primary Reached CRV;z position (mm);x position (mm)");
      canv->Update();
      canv->SaveAs(("../Plots/crvinfomcplane_primaryZ_vs_X" + cutIdentifier + filetype).c_str());

    }

  //Clean up
  canv->Close();
  logCanv->Close();
  deleteHists();

}

