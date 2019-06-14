//Produce comparison plots of CRY-1, CRY-2, and DBY
//Based off compare_CRY_DBY.C

TTree *tree_cry1;
TTree *tree_cry2;
TTree *tree_dby;

/**********************************************************************************************************************************************************************************/
//////////////////////////////////////////////////////////////    Define Cuts    ///////////////////////////////////////////////////////////////////////////////////////////////////

//Signal cuts
string momentum_cut = "deent.mom>100 && deent.mom<110 && ue.nhits<0"; 
//string momentum_cut = "ue.nhits<0";
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
string expandedMomentum = "ue.nhits<0&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R + "&&" + trk_cut_pid;

//Alternative cuts
string d0is0 = "demcent.d0==0";
string noMomNoPID = trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R + "&&" + max_trans_R;
string testCut = "ue.nhits<0&&" + trk_cuts_MDC + "&&" + pitch_angle + "&&" + min_trans_R;


/**********************************************************************************************************************************************************************************/
//////////////////////////////////////////////////////    Make a single overlaid plot    ///////////////////////////////////////////////////////////////////////////////////////////

void makePlots(std::string variable, string xTitle, int cut_version=0){
  gStyle->SetOptStat(0); 

  TCanvas *canv = new TCanvas("canv","CRY-1, CRY-2, & DBY Comparisons",1800,600);

  TCut cuts;
  if (cut_version == 1) //Signal cuts
    cuts = TCut(all_cuts_MDC_pid.c_str());
  else if (cut_version == 2) //Expanded momentum window
    cuts = TCut(expandedMomentum.c_str());    
  else //No cuts by default
    cuts = "";
    
  if (variable == "cos(theta)")
    {
      tree_cry1->Draw(Form("%s>>hCRY1", "demcpri.momz / sqrt((demcpri.momx * demcpri.momx) + (demcpri.momy * demcpri.momy) + (demcpri.momz * demcpri.momz))"), cuts, "goff");
      tree_cry2->Draw(Form("%s>>hCRY2", "demcpri.momz / sqrt((demcpri.momx * demcpri.momx) + (demcpri.momy * demcpri.momy) + (demcpri.momz * demcpri.momz))"), cuts, "goff");
      tree_dby->Draw(Form("%s>>hDBY", "demcpri.momz / sqrt((demcpri.momx * demcpri.momx) + (demcpri.momy * demcpri.momy) + (demcpri.momz * demcpri.momz))"), cuts, "goff");
    }
  else
    {
      tree_cry1->Draw(Form("%s>>hCRY1", variable.c_str()), cuts, "goff");
      tree_cry2->Draw(Form("%s>>hCRY2", variable.c_str()), cuts, "goff");
      tree_dby->Draw(Form("%s>>hDBY", variable.c_str()), cuts, "goff");
    }
  TH1F *hCRY1 = (TH1F*)gDirectory->Get("hCRY1");
  TH1F *hCRY2 = (TH1F*)gDirectory->Get("hCRY2");
  TH1F *hDBY = (TH1F*)gDirectory->Get("hDBY");

  hCRY1->SetLineColor(kBlue);
  hCRY2->SetLineColor(kBlack);
  hDBY->SetLineColor(kRed);
  // hCRY1->SetFillColorAlpha(kBlue, 0.1);
  // hCRY2->SetFillColorAlpha(kBlack, 0.1);
  // hDBY->SetFillColorAlpha(kRed, 0.1);
  // hCRY1->SetFillStyle(3004);
  // hCRY2->SetFillStyle(3005);
  // hDBY->SetFillStyle(3002);

  hCRY1->Scale(1.0 / hCRY1->Integral());
  hCRY2->Scale(1.0 / hCRY2->Integral());
  hDBY->Scale(1.0 / hDBY->Integral());

  hCRY1->SetTitle(variable.c_str());
  hCRY2->SetTitle(variable.c_str());
  hDBY->SetTitle(variable.c_str());
  hCRY1->SetXTitle(xTitle.c_str());
  hCRY2->SetXTitle(xTitle.c_str());
  hDBY->SetXTitle(xTitle.c_str());
  hCRY1->GetYaxis()->SetCanExtend(true); 
  hCRY2->GetYaxis()->SetCanExtend(true);
  hDBY->GetYaxis()->SetCanExtend(true);

  double maxCRY1 = hCRY1->GetBinContent(hCRY1->GetMaximumBin());
  double maxCRY2 = hCRY2->GetBinContent(hCRY2->GetMaximumBin());
  double maxDBY = hDBY->GetBinContent(hDBY->GetMaximumBin()); 
  if (maxDBY > maxCRY1 && maxDBY > maxCRY2)
    {
      hDBY->Draw("hist");
      hCRY1->Draw("hist same");
      hCRY2->Draw("hist same");
      }
  else if (maxCRY2 > maxDBY && maxCRY2 > maxCRY2)
    {
      hCRY2->Draw("hist");
      hCRY1->Draw("hist same");
      hDBY->Draw("hist same");
    }  
  else
    {
      hCRY1->Draw("hist");
      hCRY2->Draw("hist same");
      hDBY->Draw("hist same");
    }  

  TLegend *leg = new TLegend(0.7,0.7,0.85,0.9,"Entries");
  leg->AddEntry(hCRY2,Form("CRY-2: %d", (int)hCRY2->GetEntries()),"l");
  leg->AddEntry(hCRY1,Form("CRY-1: %d", (int)hCRY1->GetEntries()),"l");
  leg->AddEntry(hDBY,Form("DBY: %d", (int)hDBY->GetEntries()),"l");
  leg->Draw("same");

  std::string plot_name;
  plot_name.reserve(variable.size());
  for(size_t i = 0; i < variable.size(); ++i)
    {
      if(variable[i] != ']' && variable[i] != '[' && variable[i] != '_') 
	plot_name += variable[i];
    }

  string cutID;
  if (cut_version == 1)
    cutID = "_signalCuts";
  else if (cut_version == 2)
    cutID = "_expandedMom";
  else
    cutID = "";

  canv->SaveAs(Form("comparisonPlots/%s%s.pdf", plot_name.c_str(), cutID.c_str()));
  canv->SetLogy();
  canv->SaveAs(Form("comparisonPlots/%s%s_log.pdf", plot_name.c_str(), cutID.c_str()));
 
  canv->Close();
  hCRY1->Delete();
  hCRY2->Delete();
  hDBY->Delete();

}

/**********************************************************************************************************************************************************************************/
/////////////////////////////////////////////////////////////   Make All Plots    //////////////////////////////////////////////////////////////////////////////////////////////////

void compareAll(){
  TFile file_cry1("/mu2e/data/users/bbarton/CRY1/TrkAnaTrees/cry1_unmixedTrkAna.root");
  TFile file_cry2("/mu2e/data/users/bbarton/CRY2/TrkAnaTrees/cry2_trkana.root");
  TFile file_dby("/mu2e/data/users/bbarton/DBY/TrkAnaTrees/dby_trkana.root");
  tree_cry1 = (TTree*) file_cry1.Get("TrkAnaNeg/trkana");
  tree_cry2 = (TTree*) file_cry2.Get("TrkAnaNeg/trkana");
  tree_dby = (TTree*) file_dby.Get("TrkAnaNeg/trkana");
  
  tree_dby->SetLineColor(kRed);
  tree_dby->SetMarkerColor(kRed);
  tree_cry1->SetMarkerStyle(6);
  tree_cry1->SetMarkerColor(kBlue);
  tree_cry1->SetLineColor(kBlue);
  tree_cry2->SetMarkerColor(kBlack);
  tree_cry2->SetLineColor(kBlack);

  std::vector<int> cut_versions = {0, 1, 2}; //No cuts + signal cuts + expanded momentum window cuts
  for (int cutv = 0; cutv < cut_versions.size(); cutv++)
    {
      makePlots("crvinfomc._x[0]", "MC Truth: x at CRV (mm)", cut_versions.at(cutv)); //x at CRV
      makePlots("crvinfomc._y[0]", "MC Truth: y at CRV (mm)", cut_versions.at(cutv)); //y at CRV
      makePlots("crvinfomc._z[0]", "MC Truth: z at CRV (mm)", cut_versions.at(cutv)); //z at CRV
      makePlots("crvinfomc._primaryX", "Primary Particle x (mm)", cut_versions.at(cutv)); //Primary x
      makePlots("crvinfomc._primaryY", "Primary Particle y (mm)", cut_versions.at(cutv)); //Primary y
      makePlots("crvinfomc._primaryZ", "Primary Particle z (mm)", cut_versions.at(cutv)); //Primary z
      makePlots("crvinfomc._primaryPdgId", "Primary particle PDG ID", cut_versions.at(cutv)); //Primary PDG ID
      makePlots("crvinfomc._primaryE", "Energy of Primary Particle (MeV)", cut_versions.at(cutv)); //Primary Energy
      makePlots("demcgen.pdg", "Mother particle PDG ID", cut_versions.at(cutv)); //Mother particle PDG ID
      makePlots("demc.pdg", "Reconstructed Particle PDG ID", cut_versions.at(cutv)); //Reconstructed particle PDG ID
      makePlots("deent.mom", "Momentum (MeV/c)", cut_versions.at(cutv)); //Momentum
      makePlots("deent.d0", "Downstream e^- : Distance from Center", cut_versions.at(cutv)); //d0
      makePlots("deent.td", "Downstream e^- : Pitch Angle", cut_versions.at(cutv)); // Pitch angle
      makePlots("de.t0", "Downstream e^- : t0", cut_versions.at(cutv)); //t0
      makePlots("dequal._trackQualDeM", "Track Quality", cut_versions.at(cutv)); //Track quality
      makePlots("demcent.d0", "MC Truth : Downstream e^- : Distance from Center", cut_versions.at(cutv)); // MC Truth d0
      makePlots("cos(theta)","cos(#theta)", cut_versions.at(cutv)); //cos(theta)
    }

}
