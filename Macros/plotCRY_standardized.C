//Standardized plotting of CRY samples from TrkAna Trees
//Ben Barton


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

string momentum_cut = "de.mom>100 && de.mom<110 && ue.nhits<0"; 
string trk_cuts_MDC = "de.trkqual>0.4";
string pitch_angle  = "de.td>0.57735027 && de.td<1"; //  Excludes beam particles
string min_trans_R  = "de.d0>-80 && de.d0<105"; //  Consistent with coming from the target
string max_trans_R  = "(demcent.d0+2.0/demcent.om)>450. && (demcent.d0+2.0/demcent.om)<680."; //  Inconsistent with hitting the proton absorber
string timing_cut   = "de.t0>700 && de.t0<1600"; //unnecessary for unmixed samples
string all_cuts_MDC = momentum_cut + "&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string all_cuts_MDC_mixed = all_cuts_MDC + "&&" + timing_cut;


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
TH2F *h_crvinfomc_primaryZ_vs_Y; 
TH1F *h_crvinfomc_primaryPdgId;
TH1F *h_crvinfomc_primaryE;
TH1F *h_de_mom;
TH1F *h_de_d0;
TH1F *h_de_td;
TH1F *h_de_t0;
TH1F *h_de_trkqual;
TH1F *h_demcent_d0;
TH2F *h_det0_vs_CRVtimeWindowStart;
TH2F *h_det0_vs_crvinfomc_time;


void initializeHists(){
  //Position at CRV
  //crvinfomc._x[0] - x position at the CRV
  h_crvinfomc_x0 = new TH1F("h_crvinfomc_x0", "crvinfomc._x[0]", 100, -7200, 400);
  h_crvinfomc_x0->SetXTitle("x at CRV (mm)");
  //crvinfomc._y[0] - y position at the CRV
  h_crvinfomc_y0 = new TH1F("h_crvinfomc_y0", "crvinfomc._y[0]", 100, -2000, 3200);
  h_crvinfomc_y0->SetXTitle("y at CRV (mm)");
  //crvinfomc._z[0] - z position at the CRV
  h_crvinfomc_z0 = new TH1F("h_crvinfomc_z0", "crvinfomc._z[0]", 100, -3000, 19000);
  h_crvinfomc_z0->SetXTitle("z at CRV (mm)");
  //crvinfomc._z[0]:crvinfomc._x[0] - z vs x position at the CRV
  h_crvinfomc_z0_vs_x0 = new TH2F("h_crvinfomc_z0_vs_x0","crvinfomc._z[0] vs crvinfomc._x[0]", 100, -7200, 400, 100, -3000, 19000);
  h_crvinfomc_z0_vs_x0->SetXTitle("x at the CRV (mm)");
  h_crvinfomc_z0_vs_x0->SetYTitle("z at the CRV (mm)");
  h_crvinfomc_z0_vs_x0->SetStats(false);
  //crvinfomc._z[0]:crvinfomc._y[0] - z vs y position at the CRV
  h_crvinfomc_z0_vs_y0 = new TH2F("h_crvinfomc_z0_vs_y0","crvinfomc._z[0] vs crvinfomc._y[0]", 100, -2000, 3200, 100, -3000, 19000);
  h_crvinfomc_z0_vs_y0->SetXTitle("y at the CRV (mm)");
  h_crvinfomc_z0_vs_y0->SetYTitle("z at the CRV (mm)");
  h_crvinfomc_z0_vs_y0->SetStats(false);

  //Primary Particle
  //crvinfomc._primaryX - x position of the primary particle
  h_crvinfomc_primaryX = new TH1F("crvinfomc_primaryX", "crvinfomc._primaryX", 100, -300, 300);
  h_crvinfomc_primaryX->SetXTitle("x (mm)");
  //crvinfomc._primaryY - y position of the primary particle
  h_crvinfomc_primaryY = new TH1F("crvinfomc_primaryY", "crvinfomc._primaryY", 100, -2000, 16000);
  h_crvinfomc_primaryY->SetXTitle("y (mm)");
  //crvinfomc._primaryZ - z position of the primary particle
  h_crvinfomc_primaryZ = new TH1F("crvinfomc_primaryZ", "crvinfomc._primaryZ", 100, -300, 400);
  h_crvinfomc_primaryZ->SetXTitle("z (mm)");
  //cvinfomc._primaryZ:crvinfomc._primaryX - z vs x position of the primary
  h_crvinfomc_primaryZ_vs_X = new TH2F("h_crvinfomc_primaryZ_vs_X", "crvinfomc._primaryZ vs crvinfomc._primaryX", 100, -300, 300, 100, -300, 400);
  h_crvinfomc_primaryZ_vs_X->SetXTitle("x (mm)");
  h_crvinfomc_primaryZ_vs_X->SetYTitle("z (mm)");
  h_crvinfomc_primaryZ_vs_X->SetStats(false);
  //cvinfomc._primaryZ:crvinfomc._primaryX - z vs y position of the primary
  h_crvinfomc_primaryZ_vs_Y = new TH2F("h_crvinfomc_primaryZ_vs_Y", "crvinfomc._primaryZ vs crvinfomc._primaryY", 100, -300, 300, 100, -2000, 16000);
  h_crvinfomc_primaryZ_vs_Y->SetXTitle("y (mm)");
  h_crvinfomc_primaryZ_vs_Y->SetYTitle("z (mm)");
  h_crvinfomc_primaryZ_vs_Y->SetStats(false);
  //crvinfomc._primaryPgdId - Primary particle ID
  h_crvinfomc_primaryPdgId = new TH1F("h_crvinfomc_primaryPdgId", "crvinfomc._primaryPdgId", 100, -300, 2300);
  h_crvinfomc_primaryPdgId->SetXTitle("Primary Particle Pdg ID");
  //crvinfomc._primaryE - Energy of primary
  h_crvinfomc_primaryE = new TH1F("h_crvinfomc_primaryE", "h_crvinfomc._primaryE", 100, 0, 1000);
  h_crvinfomc_primaryE->SetXTitle("Energy (Mev)");

  //Downstream Electron
  //de.mom - Momentum of downstream electron
  h_de_mom = new TH1F("h_de_mom", "de.mom", 100, 50, 200);
  h_de_mom->SetXTitle("Momentum (MeV/c)");
  //de.do - Distance of downstream electron from the center of the detector
  h_de_d0 = new TH1F("h_de_d0", "de.d0", 100, -500, 500);
  h_de_d0->SetXTitle("Distance to Center (mm)");
  //de.td - Pitch angle of downstream electron
  h_de_td = new TH1F("h_de_td", "de.td", 100, 0, 5);
  h_de_td->SetXTitle("Pitch Angle");
  //de.t0 - t_0 of downstream electron track
  h_de_t0 = new TH1F("h_de_t0", "de.t0", 100, 700, 1600);
  h_de_t0->SetXTitle("Time (ns)");
  //de.trkqual - Track quality score of downstream electron track
  h_de_trkqual = new TH1F("h_de_trkqual", "de.trkqual", 100, 0, 1);
  h_de_trkqual->SetXTitle("Track Quality Rating");
  //demcent.d0 - MC truth downstream electron distance at tracker entrance
  h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -800, 600);
  h_demcent_d0->SetXTitle("Distance from center (mm)");

  //Timing Comparisons
  //de.t0:crvinfo._timeWindowStart - t0 of tracker vs t0 of CRV
  h_det0_vs_CRVtimeWindowStart = new TH2F("h_det0_vs_CRVtimeWindowStart","de.t0:crvinfo._timeWindowStart", 100, 400, 1800, 100, 700, 1600);
  h_det0_vs_CRVtimeWindowStart->SetXTitle("t_0 at CRV (ns)");
  h_det0_vs_CRVtimeWindowStart->SetYTitle("t_0 at Tracker (ns)");
  h_det0_vs_CRVtimeWindowStart->SetStats(false);
  //de.t0:crvinfomc._time - MC Truth time at tracker vs CRV
  h_det0_vs_crvinfomc_time = new TH2F("h_det0_vs_crvinfomc_time","de.t0:crvinfomc._time", 100, 0, 1, 100, 700, 1600);
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
  h_crvinfomc_primaryZ_vs_Y->Delete();
  h_crvinfomc_primaryPdgId->Delete();
  h_crvinfomc_primaryE->Delete();
  h_de_mom->Delete();
  h_de_d0->Delete();
  h_de_td->Delete();
  h_de_t0->Delete();
  h_de_trkqual->Delete();
  h_demcent_d0->Delete();
  h_det0_vs_CRVtimeWindowStart->Delete();
  h_det0_vs_crvinfomc_time->Delete();
}


/********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Make Standardized Plots    ////////////////////////////////////////////////////////////////////////

//Plot all standardized histograms
void makeStandardizedPlots(string treePath, bool neg, bool mixed)
{
  initializeHists();

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
  if (mixed)
    cuts = TCut(all_cuts_MDC_mixed.c_str());
  else
    cuts = TCut(all_cuts_MDC.c_str());
  
  //Choose which stats to displace
  // gStyle->SetOptStat(001110110);

  //Fill and plot and save histograms

  //crvinfomc_x0
  tree->Draw("crvinfomc._x[0]>>h_crvinfomc_x0","", "");
  h_crvinfomc_x0 = (TH1F*) gDirectory->Get("h_crvinfomc_x0");
  canv->cd();
  h_crvinfomc_x0->Draw("hist");
  canv->SaveAs("standardizedPlots/crvinfomc_x0.pdf");
  logCanv->cd();
  h_crvinfomc_x0->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_x0_logY.pdf");
  /*
  //crvinfomc_y0
  tree->Draw("crvinfomc._y[0]>>+h_crvinfomc_y0",cuts, "goff");
  canv->cd();
  h_crvinfomc_y0->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_y0.pdf");
  logCanv->cd();
  h_crvinfomc_y0->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_y0_logY.pdf");

  //crvinfomc_z0
  tree->Draw("crvinfomc._z[0]>>+h_crvinfomc_z0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_z0.pdf");
  logCanv->cd();
  h_crvinfomc_z0->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_z0_logY.pdf");

  //h_crvinfomc_z0_vs_x0
  tree->Draw("crvinfomc._x[0]:crvinfomc._z[0]>>+h_crvinfomc_z0_vs_x0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0_vs_x0->Draw();
  canv->SaveAs("standardizedPlots/h_crvinfomc_z0_vs_x0.pdf");
  logCanv->cd();
  h_crvinfomc_z0_vs_x0->Draw();
  logCanv->SaveAs("standardizedPlots/h_crvinfomc_z0_vs_x0_logY.pdf");

  //h_crvinfomc_z0_vs_y0
  tree->Draw("crvinfomc._y[0]:crvinfomc._z[0]>>+h_crvinfomc_z0_vs_y0",cuts, "goff");
  canv->cd();
  h_crvinfomc_z0_vs_y0->Draw();
  canv->SaveAs("standardizedPlots/h_crvinfomc_z0_vs_y0.pdf");
  logCanv->cd();
  h_crvinfomc_z0_vs_y0->Draw();
  logCanv->SaveAs("standardizedPlots/h_crvinfomc_z0_vs_y0_logY.pdf");
  
  //crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX>>+h_crvinfomc_primaryX",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryX->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryX.pdf");
  logCanv->cd();
  h_crvinfomc_primaryX->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryX_logY.pdf");

  //crvinfomc._primaryY
  tree->Draw("crvinfomc._primaryY>>+h_crvinfomc_primaryY",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryY->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryY.pdf");
  logCanv->cd();
  h_crvinfomc_primaryY->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryY_logY.pdf");

  //crvinfomc._primaryZ
  tree->Draw("crvinfomc._primaryZ>>+h_crvinfomc_primaryZ",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryZ->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryZ.pdf");
  logCanv->cd();
  h_crvinfomc_primaryZ->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryZ_logY.pdf");

  //cvinfomc._primaryZ:crvinfomc._primaryX
  tree->Draw("crvinfomc._primaryX:crvinfomc._primaryZ",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryZ_vs_X->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryZ_vs_X.pdf");
  logCanv->cd();
  h_crvinfomc_primaryZ_vs_X->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryZ_vs_X_logY.pdf");

  //cvinfomc._primaryZ:crvinfomc._primaryY
  tree->Draw("crvinfomc._primaryY:crvinfomc._primaryZ",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryZ_vs_Y->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryZ_vs_Y.pdf");
  logCanv->cd();
  h_crvinfomc_primaryZ_vs_Y->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryZ_vs_Y_logY.pdf");

  //crvinfomc._primaryPdgId
  tree->Draw("crvinfomc._primaryPdgId>>+h_crvinfomc_primaryPdgId",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryPdgId.pdf");
  logCanv->cd();
  h_crvinfomc_primaryPdgId->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryPdgId_logY.pdf");
  
  //crvinfomc._primaryE
  tree->Draw("crvinfomc._primaryE>>+h_crvinfomc_primaryE",cuts, "goff");
  canv->cd();
  h_crvinfomc_primaryE->Draw();
  canv->SaveAs("standardizedPlots/crvinfomc_primaryE.pdf");
  logCanv->cd();
  h_crvinfomc_primaryE->Draw();
  logCanv->SaveAs("standardizedPlots/crvinfomc_primaryE_logY.pdf");

  //de.mom
  tree->Draw("de.mom>>+h_de_mom",cuts, "goff");
  canv->cd();
  h_de_mom->Draw();
  canv->SaveAs("standardizedPlots/de_mom.pdf");
  logCanv->cd();
  h_de_mom->Draw();
  logCanv->SaveAs("standardizedPlots/de_mom_logY.pdf");

  //de.d0
  tree->Draw("de.d0>>+h_de_d0",cuts, "goff");
  canv->cd();
  h_de_d0->Draw();
  canv->SaveAs("de_d0.pdf");
  logCanv->cd();
  h_de_d0->Draw();
  logCanv->SaveAs("de_d0_logY.pdf");

  //de.td
  tree->Draw("de.td>>+h_de_td",cuts, "goff");
  canv->cd();
  h_de_td->Draw();
  canv->SaveAs("de_td.pdf");
  logCanv->cd();
  h_de_td->Draw();
  logCanv->SaveAs("de_td_logY.pdf");

  //de.t0
  tree->Draw("de.t0>>+h_de_t0",cuts, "goff");
  canv->cd();
  h_de_t0->Draw();
  canv->SaveAs("standardizedPlots/de_t0.pdf");
  logCanv->cd();
  h_de_t0->Draw();
  logCanv->SaveAs("standardizedPlots/de_t0_logY.pdf");

  //de.trkqual
  tree->Draw("de.trkqual>>+h_de_trkqual",cuts, "goff");
  canv->cd();
  h_de_trkqual->Draw();
  canv->SaveAs("standardizedPlots/de_trkqual.pdf");
  logCanv->cd();
  h_de_trkqual->Draw();
  logCanv->SaveAs("standardizedPlots/de_trkqual_logY.pdf");

  //demcent.d0
  tree->Draw("demcent.d0>>+h_demcent_d0",cuts, "goff");
  canv->cd();
  h_demcent_d0->Draw();
  canv->SaveAs("standardizedPlots/demcent_d0.pdf");
  logCanv->cd();
  h_demcent_d0->Draw();
  logCanv->SaveAs("standardizedPlots/demcent_d0_logY.pdf");

  //de.t0:crvinfo._timeWindowStart
   tree->Draw("de.t0:crvinfo._timeWindowStart>>+h_det0_vs_CRVtimeWindowStart",cuts, "goff");
  canv->cd();
  h_det0_vs_CRVtimeWindowStart->Draw();
  canv->SaveAs("standardizedPlots/det0_vs_CRVtimeWindowStart.pdf");
  logCanv->cd();
  h_det0_vs_CRVtimeWindowStart->Draw();
  logCanv->SaveAs("standardizedPlots/det0_vs_CRVtimeWindowStart_logY.pdf");
  
  //de.t0:crvinfomc._time
   tree->Draw("de.t0:crvinfomc._time>>+h_det0_vs_crvinfomc_time",cuts, "goff");
  canv->cd();
  h_det0_vs_crvinfomc_time->Draw();
  canv->SaveAs("standardizedPlots/det0_vs_crvinfomc_time.pdf");
  logCanv->cd();
  h_det0_vs_crvinfomc_time->Draw();
  logCanv->SaveAs("standardizedPlots/det0_vs_crvinfomc_time_logY.pdf");

  */
  canv->Close();
  logCanv->Close();
  deleteHists();

}


