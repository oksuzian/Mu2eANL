//Comparing CRY and Daya Bay MC simulations
//Modified from PlotCRY.C

#include <boost/algorithm/string/replace.hpp>
#include <algorithm>

TTree *tree_cry;
TTree *tree_dby;

void Compare_two_plots(std::string variable, const char* cut, int cut_version=0){
  gStyle->SetOptStat(0); 

  TCanvas *c1 = new TCanvas("c1","c1",1800,600);

  tree_cry->Draw(Form("%s>>hCRY", variable.c_str()),cut, "goff");
  tree_dby->Draw(Form("%s>>hDBY", variable.c_str()),cut, "goff");
  TH1F *hCRY = (TH1F*)gDirectory->Get("hCRY");
  TH1F *hDBY = (TH1F*)gDirectory->Get("hDBY");

  hCRY->SetLineColor(kBlue);
  hDBY->SetLineColor(kRed);
  hCRY->SetNormFactor(1.);
  hDBY->SetNormFactor(1.);
  hCRY->SetYTitle("Count (normalized)");
  hDBY->SetYTitle("Count (normalized)");
  hCRY->GetYaxis()->SetCanExtend(true);  

  /*
  string xTitle = "";
  cout << "Enter a title for the x-axis for variable = " << variable << endl;
  cin >> xTitle;
  // xTitle = getXaxisTitle(variable);
  hCRY->SetXTitle(xTitle.c_str());
  hDBY->SetXTitle(xTitle.c_str());
  */

  double maxCRY = hCRY->GetBinContent(hCRY->GetMaximumBin())*hCRY->Integral();
  double maxDBY = hCRY->GetBinContent(hDBY->GetMaximumBin())*hDBY->Integral();
  
  if (maxDBY > maxCRY)
    {
      cout <<"maxCRY = " << maxCRY << " and maxDBY = " << maxDBY<< endl;
      hDBY->Draw();
      hCRY->Draw("same");
      }
  else
    {
      hCRY->Draw();
      hDBY->Draw("same");
    }  

  TLegend *leg = new TLegend(0.8,0.8,0.9,0.9);
  leg->AddEntry(hCRY,Form("CRY: %d", (int)hCRY->GetEntries()),"l");
  leg->AddEntry(hDBY,Form("DBY: %d", (int)hDBY->GetEntries()),"l");
  leg->Draw("same");

  std::string plot_name;
  plot_name.reserve(variable.size());
  for(size_t i = 0; i < variable.size(); ++i)
    if(variable[i] != ']' && variable[i] != '[' && variable[i] != '_') plot_name += variable[i];

  c1->SaveAs(Form("plots/%s_%d.pdf", plot_name.c_str(), cut_version));
  /*
  string buffer = "";
  cout << "Enter any character to see log plot" <<endl;
  cin >> buffer;*/
  c1->SetLogy();
  c1->SaveAs(Form("plots/%s_%d_log.pdf", plot_name.c_str(), cut_version));
  hCRY->Delete();
  hDBY->Delete();
  //c1->Close();

}

void Compare_two_graphs(const char* variable, const char* name, const char* cut1, const char* cut2,int cut_version=0){

  TCanvas *c1 = new TCanvas("c1","c1",1800,600);
  tree_cry->Draw(variable, cut2);
  TGraph *gr2 = new TGraph(tree_cry->GetSelectedRows(), tree_cry->GetV2(), tree_cry->GetV1());
  tree_cry->Draw(variable, cut1);
  TGraph *gr1 = new TGraph(tree_cry->GetSelectedRows(), tree_cry->GetV2(), tree_cry->GetV1());


  gr1->SetMarkerStyle(6);
  gr2->SetMarkerStyle(20);
  gr2->SetMarkerColor(kRed);

  gr1->Draw("p"); //draw graph in current pad
  gr2->Draw("p same"); //draw graph in current pad

  c1->SaveAs(Form("plots/var%s_%d.png", name, cut_version));
  c1->SaveAs(Form("plots/var%s_%d.pdf", name, cut_version));
}


void compare_CRY_DBY(){
  TFile file1("/mu2e/data/users/oksuzian/nts.oksuzian.trkanadigi.trkana_prov743_test1.root");
  TFile file2("/mu2e/data/users/oksuzian/nts.oksuzian.trkana.mix.cry1.root");
  tree_cry = (TTree*) file2.Get("TrkAnaNeg/trkana");
  tree_dby = (TTree*) file1.Get("TrkAnaNeg/trkana");
  //  tree_pos = (TTree*) file1.Get("TrkAnaPos/trkana");

  tree_dby->SetLineColor(kRed);
  tree_dby->SetMarkerColor(kRed);
  tree_cry->SetMarkerStyle(6);
  tree_cry->SetMarkerColor(kBlue);
  tree_cry->SetLineColor(kBlue);


  string momentum_cut = "deent.mom>100 && deent.mom<110 && ue.nhits<0"; 
  //string momentum_cut = "ue.nhits<0"; //No deent mom requirement
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
  string noCuts = "";

  std::vector<std::string> cuts = {all_cuts_MDC_pid};
  for (int n=0; n<cuts.size(); n++){
    std::cout << "Cut: " << cuts[n] << std::endl;

    //Compare_two_graphs("crvinfomc._x[0]:crvinfomc._z[0]", "xz", cuts[n].c_str(), signal_all.c_str(), n);
    //Compare_two_graphs("crvinfomc._y[0]:crvinfomc._z[0]", "yz", cuts[n].c_str(), signal_all.c_str(), n);

    // Compare_two_plots("evtinfo.subrunid+100000*evtinfo.eventid",cuts[n].c_str(), n);
    // Compare_two_plots("deent.mom",cuts[n].c_str(), n);
    // Compare_two_plots("demc.pmom", cuts[n].c_str(), n);
    // Compare_two_plots("deent.td", cuts[n].c_str(), n);
    // Compare_two_plots("deent.d0", cuts[n].c_str(), n);
    // Compare_two_plots("de.t0", cuts[n].c_str(), n);
    // Compare_two_plots("demcent.d0", cuts[n].c_str(), n);
    // Compare_two_plots("demcent.om", cuts[n].c_str(), n);
    // Compare_two_plots("dec.eclust", cuts[n].c_str(), n);
    // Compare_two_plots("dec.uvchisq", cuts[n].c_str(), n);
    // Compare_two_plots("dec.dtllr", cuts[n].c_str(), n);
    // Compare_two_plots("dec.epllr", cuts[n].c_str(), n);
    // Compare_two_plots("de.trkqual", cuts[n].c_str(), n);
    // Compare_two_plots("de.p0err", cuts[n].c_str(), n);
    // Compare_two_plots("de.con", cuts[n].c_str(), n);
    // Compare_two_plots("deent.t0err", cuts[n].c_str(), n);
    // Compare_two_plots("demc.pdg", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ppdg", cuts[n].c_str(), n);
    // Compare_two_plots("demc.prpdg", cuts[n].c_str(), n);
    // Compare_two_plots("demc.nambig", cuts[n].c_str(), n);
    Compare_two_plots("demc.ngood", cuts[n].c_str(), n);
    // Compare_two_plots("demc.nactive", cuts[n].c_str(), n);
    Compare_two_plots("demc.nhits", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ndigigood", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ndigi", cuts[n].c_str(), n);    
    // Compare_two_plots("crvinfomc._x[0]", cuts[n].c_str(), n);
    // Compare_two_plots("crvinfomc._y[0]", cuts[n].c_str(), n);
    // Compare_two_plots("crvinfomc._z[0]", cuts[n].c_str(), n);

    string buffer = "";
    cout << "Enter any character to make the next plot" <<endl;
    cin >> buffer;;
  }

  // TCanvas *c1 = new TCanvas("c1","c1",1800,600);
  // tree_neg->Draw("demc.prpdg", "de.mom>0 && ncrv==0 && abs(demc.prpdg)!=13");
  // c1->SaveAs("plots/PrimaryPDG_neg.pdf");
  //tree_dby->Draw("demcgen.pdg", "de.mom>0 && abs(demcgen.pdg)!=13");
  // c1->SaveAs("plots/PrimaryPDG_pos.pdf");

  // tree_cry->Draw("demc.pdg", signal_all.c_str(), "hist TEXT");
  // c1->SaveAs("plots/PDG_neg.pdf");
  //tree_cry->Draw("ncrv", signal_all.c_str(), "hist TEXT");
  // c1->SaveAs("plots/ncrv_neg.pdf");
  // tree_dby->Draw("demc.pdg", signal_all.c_str(), "hist TEXT");
  // c1->SaveAs("plots/PDG_pos.pdf");

  //((TTreePlayer*)(tree_cry->GetPlayer()))->SetScanRedirect(true); 
  // ((TTreePlayer*)(tree_cry->GetPlayer()))->SetScanFileName("signal_events.txt"); 
  //tree_cry->Scan("de.mom:demc.pdg:evtinfo.eventid:evtinfo.subrunid:@crvinfo.size():demcgen.pdg", signal_all.c_str());

  // tree_cry->Draw("crvinfomc._x[0]:crvinfomc._z[0]", "de.mom>100", "same");
  // c1->SaveAs("plots/XZ_cry.png");
}

//Function to make a single plot
void plotHist(string path, string variable, bool neg)
{

  // Cuts
  std::string momentum_cut = "de.mom>100 && de.mom<110 && ue.nhits<0";
  // std::string momentum_cut = "de.mom>80 && de.mom<120 && ue.nhits<0";
  std::string trk_cuts_CD3 = "de.nactive>=25 && de.con>2e-3 && de.p0err<0.25 && de.t0err<0.9";
  std::string trk_cuts_MDC = "de.trkqual>0.4";
  std::string pitch_angle  = "de.td>0.57735027 && de.td<1"; //  Excludes beam particles
  //  std::string pitch_angle  = "de.td>0.57735027 && de.td<1.5"; //  Excludes beam particles
  std::string min_trans_R  = "de.d0>-80 && de.d0<105"; //  Consistent with coming from the target
  std::string max_trans_R  = "(demcent.d0+2.0/demcent.om)>450. && (demcent.d0+2.0/demcent.om)<680."; //  Inconsistent with hitting the proton absorber
  std::string signal_pid   = "dec.eclust>10.0 && dec.eclust<120 && dec.uvchisq<100.0 && (max(dec.dtllr,0.0)+max(dec.epllr,0.0))>1.5";
  std::string timing_cut   = "de.t0>700 && de.t0<1600";
  std::string all_cuts_CD3=momentum_cut+"&&"+trk_cuts_CD3+"&&"+pitch_angle+"&&"+min_trans_R+"&&"+max_trans_R+"&&"+signal_pid+"&&"+timing_cut;
  // std::string all_cuts_MDC=momentum_cut+"&&"+trk_cuts_MDC+"&&"+pitch_angle+"&&"+min_trans_R+"&&"+max_trans_R+"&&"+signal_pid+"&&"+timing_cut;
  std::string all_cuts_MDC=momentum_cut+"&&"+trk_cuts_MDC+"&&"+pitch_angle+"&&"+min_trans_R+"&&"+max_trans_R;

  // std::string signal_all = all_cuts_MDC+"&& crvinfomc._z[0]>13000";
  std::string signal_all = all_cuts_MDC;



   TCanvas *canv = new TCanvas("canv",variable.c_str(),800,600);
  //Extract a tree and histogram from the provided filename/path
  TFile inFile(path.c_str());
  TTree *tree;
  if (neg) 
    tree = (TTree*) inFile.Get("TrkAnaNeg/trkana");
  else
    tree = (TTree*) inFile.Get("TrkAnaPos/trkana");

  tree->Draw(Form("%s>>hist", variable.c_str()),signal_all.c_str(), "goff");
  TH1F *hist = (TH1F*)gDirectory->Get("hist");
 

  //Make plot look pretty
  gStyle->SetOptStat(1110);
  hist->SetTitle(variable.c_str());
  string xTitle = "";
  cout << "Enter a title for the x-axis for variable = " << variable << endl;
  cin >> xTitle;
  hist->SetXTitle(xTitle.c_str());
  hist->SetLineColor(kBlue);


  //Plot hist
  canv->cd(1);
  hist->Draw("hist");
  canv->SaveAs(Form("plots/%s.pdf", variable.c_str()));
  
  //Calculations
  cout << "Integral = " << hist->Integral(hist->FindBin(5),hist->FindBin(200)) << endl;

  // string buffer = "";
  // cout << "Enter any character to close" << endl;
  // cin >> buffer;
}
