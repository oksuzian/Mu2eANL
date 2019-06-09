//Standardized plotting of CRY samples from TrkAna Trees
//Ben Barton
//06/09/2019


/********************************************************************************************************************************************************/
///////////////////////////////////////////////////////////    Imports    ////////////////////////////////////////////////////////////////////////////////

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


/********************************************************************************************************************************************************/
////////////////////////////////////////////////////    Define Standard Cuts    //////////////////////////////////////////////////////////////////////////

string momentum_cut = "deent.mom>100 && deent.mom<110 && ue.nhits<0"; 
string trk_cuts_MDC = "dequal.TrkQualDeM>0.8"; //For original CRY1 analysis this was 0.4
string trk_cut_pid = "dequal.TrkPIDDeM>0.5";
string pitch_angle  = "deent.td>0.57735027 && deent.td<1"; //  Excludes beam particles
string min_trans_R  = "deent.d0>-80 && deent.d0<105"; //  Consistent with coming from the target
string max_trans_R  = "(demcent.d0+2.0/demcent.om)>450. && (demcent.d0+2.0/demcent.om)<680."; //  Inconsistent with hitting the proton absorber
string timing_cut   = "de.t0>700 && de.t0<1600"; //unnecessary for unmixed samples
string all_cuts_MDC = momentum_cut + "&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string all_cuts_MDC_pid = momentum_cut + "&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid;
string all_cuts_MDC_mixed = all_cuts_MDC + "&&" + timing_cut;
string all_cuts_MDC_pid_mixed = all_cuts_MDC_pid + "&&" + timing_cut;


/********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Define Standard Histograms   //////////////////////////////////////////////////////////////////////

//Position at CRV
//crvinfomc._x[0] - x position at the CRV
TH1F *h_crvinfomc_x0;
TH1F *h_crvinfomc_y0;
TH1F *h_crvinfomc_z0;
TH2F *h_crvinfomc_z0_vs_x0; 
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

void initializeHists(bool makeCuts)
{
  //Position at CRV
  //crvinfomc._x[0] - x position at the CRV
  h_crvinfomc_x0 = new TH1F("h_crvinfomc_x0", "crvinfomc._x[0]", 100, -7200, -1000); 
  h_crvinfomc_x0->SetXTitle("x at CRV (mm)");

  //crvinfomc._y[0] - y position at the CRV
  h_crvinfomc_y0 = new TH1F("h_crvinfomc_y0", "crvinfomc._y[0]", 100, -2000, 3200);
  h_crvinfomc_y0->SetXTitle("y at CRV (mm)");

  //crvinfomc._z[0] - z position at the CRV
  h_crvinfomc_z0 = new TH1F("h_crvinfomc_z0", "crvinfomc._z[0]", 100, -3000, 19000);
  h_crvinfomc_z0->SetXTitle("z at CRV (mm)");

  //crvinfomc._z[0]:crvinfomc._x[0] - z vs x position at the CRV
  h_crvinfomc_z0_vs_x0 = new TH2F("h_crvinfomc_z0_vs_x0","crvinfomc._z[0] vs crvinfomc._x[0]", 100, -5000, 20000, 100, -7500, 1000);
  h_crvinfomc_z0_vs_x0->SetXTitle("x at the CRV (mm)");
  h_crvinfomc_z0_vs_x0->SetYTitle("z at the CRV (mm)");
  h_crvinfomc_z0_vs_x0->SetStats(false);

  //crvinfomc._z[0]:crvinfomc._y[0] - z vs y position at the CRV
  h_crvinfomc_z0_vs_y0 = new TH2F("h_crvinfomc_z0_vs_y0","crvinfomc._z[0] vs crvinfomc._y[0]", 100, -5000, 20000, 100, -2500, 3500);
  h_crvinfomc_z0_vs_y0->SetXTitle("y at the CRV (mm)");
  h_crvinfomc_z0_vs_y0->SetYTitle("z at the CRV (mm)");
  h_crvinfomc_z0_vs_y0->SetStats(false);

  //Primary Particle
  //crvinfomc._primaryX - x position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryX = new TH1F("crvinfomc_primaryX", "crvinfomc._primaryX", 100, -50000, 50000);
  else
    h_crvinfomc_primaryX = new TH1F("crvinfomc_primaryX", "crvinfomc._primaryX", 100, -100000, 100000);
  h_crvinfomc_primaryX->SetXTitle("x (mm)");

  //crvinfomc._primaryY - y position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryY = new TH1F("crvinfomc_primaryY", "crvinfomc._primaryY", 100, 15365, 15366);
  else
    h_crvinfomc_primaryY = new TH1F("crvinfomc_primaryY", "crvinfomc._primaryY", 100, 15350, 15450);
  h_crvinfomc_primaryY->SetXTitle("y (mm)");

  //crvinfomc._primaryZ - z position of the primary particle
  if (makeCuts)
    h_crvinfomc_primaryZ = new TH1F("crvinfomc_primaryZ", "crvinfomc._primaryZ", 100, -40000, 40000);
  else
    h_crvinfomc_primaryZ = new TH1F("crvinfomc_primaryZ", "crvinfomc._primaryZ", 100, -100000, 100000);
  h_crvinfomc_primaryZ->SetXTitle("z (mm)");

  //cvinfomc._primaryZ:crvinfomc._primaryX - z vs x position of the primary
  h_crvinfomc_primaryZ_vs_X = new TH2F("h_crvinfomc_primaryZ_vs_X", "crvinfomc._primaryZ vs crvinfomc._primaryX", 100, -60000, 60000, 100, -60000, 60000);
  h_crvinfomc_primaryZ_vs_X->SetXTitle("x (mm)");
  h_crvinfomc_primaryZ_vs_X->SetYTitle("z (mm)");
  h_crvinfomc_primaryZ_vs_X->SetStats(false);

  //crvinfomc._primaryPgdId - Primary particle ID
  if (makeCuts)
    h_crvinfomc_primaryPdgId = new TH1F("h_crvinfomc_primaryPdgId", "crvinfomc._primaryPdgId", 100, -225, 25);
  else
    h_crvinfomc_primaryPdgId = new TH1F("h_crvinfomc_primaryPdgId", "crvinfomc._primaryPdgId", 100, -300, 2300);
  h_crvinfomc_primaryPdgId->SetXTitle("Primary Particle Pdg ID");

  //crvinfomc._primaryE - Energy of primary
  h_crvinfomc_primaryE = new TH1F("h_crvinfomc_primaryE", "h_crvinfomc._primaryE", 100, 0, 150000);
  h_crvinfomc_primaryE->SetXTitle("Energy (MeV)");

  //Downstream Electron
  //de.mom - Momentum of downstream electron
  if (makeCuts)
    h_deent_mom = new TH1F("h_deent_mom", "deent.mom", 100, 100, 110);
  else
    h_deent_mom = new TH1F("h_deent_mom", "deent.mom", 100, 0, 300);
  h_deent_mom->SetXTitle("Momentum (MeV/c)");

  //de.do - Distance of downstream electron from the center of the detector
  h_deent_d0 = new TH1F("h_deent_d0", "deent.d0", 100, -500, 500);
  h_deent_d0->SetXTitle("Distance to Center (mm)");

  //de.td - Pitch angle of downstream electron
  h_deent_td = new TH1F("h_deent_td", "deent.td", 100, 0, 5);
  h_deent_td->SetXTitle("Pitch Angle");

  //de.t0 - t_0 of downstream electron track
  h_de_t0 = new TH1F("h_de_t0", "de.t0", 100, 700, 1000);
  h_de_t0->SetXTitle("Time (ns)");

  //de.trkqual - Track quality score of downstream electron track
  if (makeCuts)
    h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQualDeM", 100, 0.4, 1);
  else
    h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQualDeM", 100, 0, 1);
  h_dequal_trkQualDeM->SetXTitle("Track Quality Rating");

  //demcent.d0 - MC truth downstream electron distance at tracker entrance
  if (makeCuts)
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -150, 150);
  else
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -500, 500);
  h_demcent_d0->SetXTitle("Distance from center (mm)");

  //Timing Comparisons
  //de.t0:crvinfo._timeWindowStart - t0 of tracker vs t0 of CRV
  h_det0_vs_CRVtimeWindowStart = new TH2F("h_det0_vs_CRVtimeWindowStart","de.t0:crvinfo._timeWindowStart", 100, 500, 1500, 100, 650, 1100);
  h_det0_vs_CRVtimeWindowStart->SetXTitle("t_0 at CRV (ns)");
  h_det0_vs_CRVtimeWindowStart->SetYTitle("t_0 at Tracker (ns)");
  h_det0_vs_CRVtimeWindowStart->SetStats(false);

  //de.t0:crvinfomc._time - MC Truth time at tracker vs CRV
  h_det0_vs_crvinfomc_time = new TH2F("h_det0_vs_crvinfomc_time","de.t0:crvinfomc._time", 100, -40, 250, 100, 700, 1050);
  h_det0_vs_crvinfomc_time->SetXTitle("MC Truth t_0 at CRV (ns)");
  h_det0_vs_crvinfomc_time->SetYTitle("t_0 at Tracker (ns)");
  h_det0_vs_crvinfomc_time->SetStats(false);

}


/********************************************************************************************************************************************************/
/////////////////////////////////////////////////////    Clean Up Histograms    //////////////////////////////////////////////////////////////////////////

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
}


/********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Make Standardized Plots    ////////////////////////////////////////////////////////////////////////

//Plot all standardized histograms
void makeStandardizedPlots(string treePath, bool neg, bool mixed, bool makeCuts)
{
  

  //Set up canvases
  TCanvas *canv = new TCanvas("canv", "CRY Plots", 800, 600);
  TCanvas *logCanv = new TCanvas("logCanv", "CRY Plots - Logscale", 800, 600);
  logCanv->SetLogy(1);

  //Read tree from file
  TFile inFile(treePath.c_str());
  TTree *tree;
  if (neg) 
    tree = (TTree*) inFile.Get("TrkAnaNeg/trkana");
  else
    tree = (TTree*) inFile.Get("TrkAnaPos/trkana");

  //Choose which cuts to use
  TCut cuts;
  string cutIdentifier;
  if (makeCuts)
    {
      if (mixed)
	cuts = TCut(all_cuts_MDC_pid_mixed.c_str());
      else
	cuts = TCut(all_cuts_MDC_pid.c_str());

      cutIdentifier = "_cut";
    }
  else
    {
      cuts = "";
      cutIdentifier = "";
    }
  
  
  //Initialize the histograms
  initializeHists(makeCuts);
  
  //Choose which stats to displace
  gStyle->SetOptStat(1110110);

  //Fill and plot and save histograms

  //crvinfomc_x0
  canv->cd();
  tree->Draw("crvinfomc._x[0]>>h_crvinfomc_x0",cuts, "");
  h_crvinfomc_x0 = (TH1F*) gDirectory->Get("h_crvinfomc_x0");
  h_crvinfomc_x0->Draw("hist");
  canv->SaveAs(("standardizedPlots/crvinfomc_x0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_x0->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_x0_logY" + cutIdentifier + ".pdf").c_str());
  
  //crvinfomc_y0
  tree->Draw("crvinfomc._y[0]>>+h_crvinfomc_y0",cuts, "goff");
  h_crvinfomc_y0 = (TH1F*) gDirectory->Get("h_crvinfomc_y0");
  canv->cd();
  h_crvinfomc_y0->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_y0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_y0->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_y0_logY" + cutIdentifier + ".pdf").c_str());

  //crvinfomc_z0
  tree->Draw("crvinfomc._z[0]>>+h_crvinfomc_z0",cuts, "goff");
  h_crvinfomc_z0 = (TH1F*) gDirectory->Get("h_crvinfomc_z0");
  canv->cd();
  h_crvinfomc_z0->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_z0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_z0->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_z0_logY" + cutIdentifier + ".pdf").c_str());

  //h_crvinfomc_z0_vs_x0
  tree->Draw("crvinfomc._x[0]:crvinfomc._z[0]>>+h_crvinfomc_z0_vs_x0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0_vs_x0->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_z0_vs_x0" + cutIdentifier + ".pdf").c_str());

  //h_crvinfomc_z0_vs_y0
  tree->Draw("crvinfomc._y[0]:crvinfomc._z[0]>>+h_crvinfomc_z0_vs_y0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0_vs_y0->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_z0_vs_y0" + cutIdentifier + ".pdf").c_str());
  
  //crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX>>+h_crvinfomc_primaryX",cuts, "goff");
  h_crvinfomc_primaryX = (TH1F*) gDirectory->Get("h_crvinfomc_primaryX");
  canv->cd();
  h_crvinfomc_primaryX->SetTitle("crvinfomc._primaryX");
  h_crvinfomc_primaryX->SetXTitle("x (mm)");
  h_crvinfomc_primaryX->SetBins(100, -100000, 100000); 
  h_crvinfomc_primaryX->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryX" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_primaryX->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_primaryX_logY" + cutIdentifier + ".pdf").c_str());

  //crvinfomc._primaryY
  tree->Draw("crvinfomc._primaryY>>+h_crvinfomc_primaryY",cuts, "goff");
  h_crvinfomc_primaryY = (TH1F*) gDirectory->Get("h_crvinfomc_primaryY");
  h_crvinfomc_primaryY->SetTitle("crvinfomc._primaryY");
  h_crvinfomc_primaryY->SetXTitle("y (mm)");
  h_crvinfomc_primaryY->SetBins(10, 15360, 15370); 
  canv->cd();
  h_crvinfomc_primaryY->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryY" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_primaryY->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_primaryY_logY" + cutIdentifier + ".pdf").c_str());

  //crvinfomc._primaryZ
  tree->Draw("crvinfomc._primaryZ>>+h_crvinfomc_primaryZ",cuts, "goff");
  h_crvinfomc_primaryZ = (TH1F*) gDirectory->Get("h_crvinfomc_primaryZ");
  h_crvinfomc_primaryZ->SetTitle("crvinfomc._primaryZ");
  h_crvinfomc_primaryZ->SetXTitle("z (mm)");
  h_crvinfomc_primaryZ->SetBins(100, -20000, 40000); 
  canv->cd();
  h_crvinfomc_primaryZ->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryZ" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_primaryZ->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_primaryZ_logY" + cutIdentifier + ".pdf").c_str());

  //cvinfomc._primaryZ:crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX:crvinfomc._primaryZ>>+h_crvinfomc_primaryZ_vs_X",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryZ_vs_X->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryZ_vs_X" + cutIdentifier + ".pdf").c_str());


  //crvinfomc._primaryPdgId
  tree->Draw("crvinfomc._primaryPdgId>>+h_crvinfomc_primaryPdgId",cuts, "goff");
  h_crvinfomc_primaryPdgId = (TH1F*) gDirectory->Get("h_crvinfomc_primaryPdgId");
  canv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryPdgId" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_primaryPdgId_logY" + cutIdentifier + ".pdf").c_str());
  
  //crvinfomc._primaryE
  tree->Draw("crvinfomc._primaryE>>+h_crvinfomc_primaryE",cuts, "goff");
  h_crvinfomc_primaryE = (TH1F*) gDirectory->Get("h_crvinfomc_primaryE");
  canv->cd();
  h_crvinfomc_primaryE->Draw();
  canv->SaveAs(("standardizedPlots/crvinfomc_primaryE" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_crvinfomc_primaryE->Draw();
  logCanv->SaveAs(("standardizedPlots/crvinfomc_primaryE_logY" + cutIdentifier + ".pdf").c_str());

  //deent.mom
  tree->Draw("deent.mom>>+h_deent_mom",cuts, "goff");
  h_deent_mom = (TH1F*) gDirectory->Get("h_deent_mom");
  canv->cd();
  h_deent_mom->Draw();
  canv->SaveAs(("standardizedPlots/deent_mom" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_deent_mom->Draw();
  logCanv->SaveAs(("standardizedPlots/deent_mom_logY" + cutIdentifier + ".pdf").c_str());

  //deent.d0
  tree->Draw("deent.d0>>+h_deent_d0",cuts, "goff");
  h_deent_d0 = (TH1F*) gDirectory->Get("h_deent_d0");
  canv->cd();
  h_deent_d0->Draw();
  canv->SaveAs(("deent_d0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_deent_d0->Draw();
  logCanv->SaveAs(("deent_d0_logY" + cutIdentifier + ".pdf").c_str());

  //deent.td
  tree->Draw("deent.td>>+h_deent_td",cuts, "goff");
  h_deent_td = (TH1F*) gDirectory->Get("h_deent_td");
  canv->cd();
  h_deent_td->Draw();
  canv->SaveAs(("deent_td" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_deent_td->Draw();
  logCanv->SaveAs(("deent_td_logY" + cutIdentifier + ".pdf").c_str());

  //de.t0
  tree->Draw("de.t0>>+h_de_t0",cuts, "goff");
  h_de_t0 = (TH1F*) gDirectory->Get("h_de_t0");
  canv->cd();
  h_de_t0->Draw();
  canv->SaveAs(("standardizedPlots/de_t0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_de_t0->Draw();
  logCanv->SaveAs(("standardizedPlots/de_t0_logY" + cutIdentifier + ".pdf").c_str());

  //dequal.TrkQualDeM
  tree->Draw("dequal.TrkQualDeM>>+h_dequal_trkQualDeM",cuts, "goff");
  h_dequal_trkQualDeM = (TH1F*) gDirectory->Get("h_dequal_trkQualDeM");
  canv->cd();
  h_dequal_trkQualDeM->Draw();
  canv->SaveAs(("standardizedPlots/dequal_trkQualDeM" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_dequal_trkQualDeM->Draw();
  logCanv->SaveAs(("standardizedPlots/de_trkQualDeM_logY" + cutIdentifier + ".pdf").c_str());

  //demcent.d0
  tree->Draw("demcent.d0>>+h_demcent_d0",cuts, "goff");
  h_demcent_d0 = (TH1F*) gDirectory->Get("h_demcent_d0");
  canv->cd();
  h_demcent_d0->Draw();
  canv->SaveAs(("standardizedPlots/demcent_d0" + cutIdentifier + ".pdf").c_str());
  logCanv->cd();
  h_demcent_d0->Draw();
  logCanv->SaveAs(("standardizedPlots/demcent_d0_logY" + cutIdentifier + ".pdf").c_str());

  //de.t0:crvinfo._timeWindowStart
  tree->Draw("de.t0:crvinfo._timeWindowStart>>+h_det0_vs_CRVtimeWindowStart",cuts, "goff");
  canv->cd();
  h_det0_vs_CRVtimeWindowStart->Draw();
  canv->SaveAs(("standardizedPlots/det0_vs_CRVtimeWindowStart" + cutIdentifier + ".pdf").c_str());
  
  //de.t0:crvinfomc._time
   tree->Draw("de.t0:crvinfomc._time>>+h_det0_vs_crvinfomc_time",cuts, "goff");
  canv->cd();
  h_det0_vs_crvinfomc_time->Draw();
  canv->SaveAs(("standardizedPlots/det0_vs_crvinfomc_time" + cutIdentifier + ".pdf").c_str());

  
  canv->Close();
  logCanv->Close();
  deleteHists();

}


