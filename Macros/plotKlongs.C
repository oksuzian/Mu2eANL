//Produced plots with predefined characteristsics from TrkAna trees of resampled K-longs
//Ben Barton
//08/02/19 
//Based off plotCRY_stanardized.C


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
///////////////////////////////////////////////////    Define Standard Histograms & Graphs    /////////////////////////////////////////////////////////////

TH1F *h_deent_mom;
TH1F *h_deent_d0;
TH1F *h_deent_td;
TH1F *h_de_t0;
TH1F *h_dequal_trkQualDeM;
TH1F *h_demcent_d0;
TH1F *h_pz_p;
TH1F *h_noCRV_demc_pdg;

TGraph *g_oposx_vs_oposz;
TGraph *g_oposy_vs_oposz;
TGraph *g_oposy_vs_oposx;


void initializeHists(bool makeCuts)
{
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
  if (makeCuts)
    h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQual", 100, 0.4, 1);
  else
    h_dequal_trkQualDeM = new TH1F("h_dequal_trkQualDeM", "dequal.TrkQual", 100, 0, 1);
  h_dequal_trkQualDeM->SetXTitle("Track Quality");
    

  //demcent.d0 - MC truth downstream electron distance at tracker entrance
  if (makeCuts)
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -150, 150);
  else
    h_demcent_d0 = new TH1F("h_demcent_d0", "demcent._d0", 100, -550, 500);
  h_demcent_d0->SetXTitle("Distance from center (mm)");

  
  //PDG ID of events with no CRV coincidences
  h_noCRV_demc_pdg = new TH1F("h_noCRV_demc_pdg", "PDG ID of Particles Producing Events with no CRV Coincidences", 100, -300, 2300);
  h_noCRV_demc_pdg->SetXTitle("PDG ID");

}


/*********************************************************************************************************************************************************/
/////////////////////////////////////////////////////    Clean Up Histograms & Graphs   ///////////////////////////////////////////////////////////////////

void deleteHists()
{
  h_deent_mom->Delete();
  h_deent_d0->Delete();
  h_deent_td->Delete();
  h_de_t0->Delete();
  h_dequal_trkQualDeM->Delete();
  h_demcent_d0->Delete();
  h_noCRV_demc_pdg->Delete();

}


/*********************************************************************************************************************************************************/
///////////////////////////////////////////////////    Make Standardized Plots    /////////////////////////////////////////////////////////////////////////

/* Produce and save plots of each histogram
   @param treePath : Path to the TrkAna tree to be plotted
   @param neg : true if plotting the negative tree, false for the positive tree
   @param makeCuts : true if cuts should be applied
   @param useMomCut : true if momentum cut should be applied (only applicable if makeCuts is true)
   @param onlyScan : If true, doesn't make plots - only performs initial events counts and scans
 */
void makeStandardizedPlots(string treePath, bool neg, bool makeCuts, bool useMomCut = true, bool onlyScan = false)
{

  /********************************************************************************************************************************************************/
  //////////////////////////////////////////////////    Define Standard Cuts    ///////////////////////////////////////////////////////////////////////////

  //Signal cuts
  string momentum_cut = "deent.mom>100 && deent.mom<110"; //Use wider momentum window than experimental signal window
  string no_upstream = "ue.status<0";
  string trk_qual = "dequal.TrkQual>0.8";; //For original CRY1 analysis this was 0.4
  string trk_cut_pid = "dequal.TrkPID>0.9";
  string pitch_angle = "deent.td>0.57735027 && deent.td<1"; //  Excludes beam particles
  string min_trans_R = "deent.d0>-80 && deent.d0<105"; //  Consistent with coming from the target
  string max_trans_R = "(deent.d0+2.0/deent.om)>450. && (deent.d0+2.0/deent.om)<680."; //  Inconsistent with hitting the proton absorber
  string timing_cut = "de.t0>700 && de.t0<1695"; // This is the standard window
  string noCRV =" @crvinfo.size()<1"; //Events without CRV hits
  string yesCRV =" @crvinfo.size()>=1"; //Events without CRV hits
  string all_cuts_MDC = momentum_cut + "&&" + no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
  string signalCuts = all_cuts_MDC + "&&" + trk_cut_pid + "&&" + timing_cut;
  string noTiming = all_cuts_MDC + "&&" + trk_cut_pid;
  string noMom = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid + "&&" + timing_cut; 
  string noTimingNoMom = no_upstream + "&&" + trk_qual + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid;
 

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
      if (useMomCut)
	cuts = TCut(signalCuts.c_str());
      else
	cuts = TCut(noMom.c_str());

      if (!neg)
	cuts = TCut(ePlusCuts.c_str());

      //cuts = TCut(noTimingNoMom.c_str());
      
      cutIdentifier = "_cut";
    }
  else
    {
      cuts = "";
      cutIdentifier = "";
    }

  
  //Print out some information about the PDG of events 
  cout << "\n##################  Events with no CRV hits  ####################" << endl;
  cout << "Reconstructed particles (demc.pdg)" << endl;
  cout << "Number of e- events = " << tree->GetEntries(cuts + "demc.pdg==11")  << endl;
  cout << "Number of e+ events = " << tree->GetEntries(cuts + "demc.pdg==-11")  << endl;
  cout << "Number of mu- events = " << tree->GetEntries(cuts + "demc.pdg==13")  << endl;
  cout << "Number of mu+ events = " << tree->GetEntries(cuts + "demc.pdg==-13")  << endl;
  cout << "Number of pi- events = " << tree->GetEntries(cuts + "demc.pdg==-211")  << endl;
  cout << "Number of pi+ events = " << tree->GetEntries(cuts + "demc.pdg==211")  << endl;
     
  cout << "\nEvents with CRV hits: " <<tree->GetEntries(cuts + "&&" +yesCRV) << "\n" << endl;

  cout << "Events which did not produce coincidences in the CRV:" << endl;
      tree->Scan("evtinfo.subrunid:evtinfo.eventid:demcgen.pdg:demc.pdg:deent.mom",cuts + "@crvinfo.size()<1","");
      cout << "\n" << endl;  

  if (onlyScan) 
    exit(0);

  //Initialize the histograms
  initializeHists(makeCuts);

  //Set up canvases
  TCanvas *canv = new TCanvas("canv", "K-longPlots", 800, 600);
  TCanvas *logCanv = new TCanvas("logCanv", "K-long Plots - Logscale", 800, 600);
  logCanv->SetLogy(1);
  
  //Choose which stats to displace
  //gStyle->SetOptStat(111110);
  gStyle->SetOptStat(1111111);

  //Choose file format to use
  string filetype = ".png";
  //string filetype = ".pdf";


  //Fill and plot and save histograms

  //deent.mom
  tree->Draw("deent.mom>>+h_deent_mom",cuts, "goff");
  h_deent_mom = (TH1F*) gDirectory->Get("h_deent_mom");
  canv->cd();
  h_deent_mom->Draw();
  canv->SaveAs(("Beam/Klongs/deent_mom" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_mom->Draw();
  logCanv->SaveAs(("Beam/Klongs/deent_mom_logY" + cutIdentifier + filetype).c_str());

  //deent.d0
  tree->Draw("deent.d0>>+h_deent_d0",cuts, "goff");
  h_deent_d0 = (TH1F*) gDirectory->Get("h_deent_d0");
  canv->cd();
  h_deent_d0->Draw();
  canv->SaveAs(("Beam/Klongs/deent_d0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_d0->Draw();
  logCanv->SaveAs(("Beam/Klongs/deent_d0_logY" + cutIdentifier + filetype).c_str());

  //deent.td
  tree->Draw("deent.td>>+h_deent_td",cuts, "goff");
  h_deent_td = (TH1F*) gDirectory->Get("h_deent_td");
  canv->cd();
  h_deent_td->Draw();
  canv->SaveAs(("Beam/Klongs/deent_td" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_deent_td->Draw();
  logCanv->SaveAs(("Beam/Klongs/deent_td_logY" + cutIdentifier + filetype).c_str());

  //de.t0
  tree->Draw("de.t0>>+h_de_t0",cuts, "goff");
  h_de_t0 = (TH1F*) gDirectory->Get("h_de_t0");
  canv->cd();
  h_de_t0->Draw();
  canv->SaveAs(("Beam/Klongs/de_t0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_de_t0->Draw();
  logCanv->SaveAs(("Beam/Klongs/de_t0_logY" + cutIdentifier + filetype).c_str());

  //dequal.TrkQualDeM or dequal.TrkQual
  tree->Draw("dequal.TrkQual>>+h_dequal_trkQualDeM",cuts, "goff");
  h_dequal_trkQualDeM = (TH1F*) gDirectory->Get("h_dequal_trkQualDeM");
  canv->cd();
  h_dequal_trkQualDeM->Draw();
  canv->SaveAs(("Beam/Klongs/dequal_trkQual" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_dequal_trkQualDeM->Draw();
  logCanv->SaveAs(("Beam/Klongs/dequal_trkQual_logY" + cutIdentifier + filetype).c_str());
   

  //demcent.d0
  tree->Draw("demcent.d0>>+h_demcent_d0",cuts, "goff");
  h_demcent_d0 = (TH1F*) gDirectory->Get("h_demcent_d0");
  canv->cd();
  h_demcent_d0->Draw();
  canv->SaveAs(("Beam/Klongs/demcent_d0" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_demcent_d0->Draw();
  logCanv->SaveAs(("Beam/Klongs/demcent_d0_logY" + cutIdentifier + filetype).c_str());

  //demc.pdg of events with no CRV coincidences
  TCut noCRVcoincidences = "@crvinfo.size()<1";
  tree->Draw("demc.pdg>>+h_noCRV_demc_pdg",cuts + noCRVcoincidences, "goff");
  h_noCRV_demc_pdg = (TH1F*) gDirectory->Get("h_noCRV_demc_pdg");
  canv->cd();
  h_noCRV_demc_pdg->Draw();
  canv->SaveAs(("Beam/Klongs/noCRV_demc_pdg" + cutIdentifier + filetype).c_str());
  logCanv->cd();
  h_noCRV_demc_pdg->Draw();
  logCanv->SaveAs(("Beam/Klongs/noCRV_demc_pdg_logY" + cutIdentifier + filetype).c_str());


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
  canv->SaveAs(("Beam/Klongs/demc_oposx_vs_oposz" + cutIdentifier + filetype).c_str());

  
  //y:z position of the electron/positron
  canv->cd();
  tree->Draw("demc.oposy:demc.oposz",("demc.pdg=="+pdgSign+"11").c_str());
  g_oposy_vs_oposz = (TGraph*) gPad->GetPrimitive("Graph");
  g_oposy_vs_oposz->SetTitle("Reconstructed e^- : Origin Position;z position (mm);y position (mm)");
  canv->Update();
  canv->SaveAs(("Beam/Klongs/demc_oposy_vs_oposz" + cutIdentifier + filetype).c_str());
  
  //y:x position of the electron/positron
  canv->cd();
  tree->Draw("demc.oposy:demc.oposx",("demc.pdg=="+pdgSign+"11").c_str());
  g_oposy_vs_oposx = (TGraph*) gPad->GetPrimitive("Graph");
  g_oposy_vs_oposx->SetTitle("Reconstructed e^- : Origin Position;x position (mm);y position (mm)");
  canv->Update();
  canv->SaveAs(("Beam/Klongs/demc_oposy_vs_oposx" + cutIdentifier + filetype).c_str());


  //Clean up
  canv->Close();
  logCanv->Close();
  deleteHists();

}

