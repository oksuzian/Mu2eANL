//Comparing CRY and Daya Bay MC simulations
//Modified from PlotCRY.C

#include <boost/algorithm/string/replace.hpp>
#include <algorithm>

TTree *tree_cry;
TTree *tree_dby;

void Compare_two_plots(std::string variable, const char* cut, int cut_version=0){

  TCanvas *c1 = new TCanvas("c1","c1",1800,600);

  tree_cry->Draw(Form("%s>>hCRY", variable.c_str()),"", "hist");
  tree_dby->Draw(Form("%s>>hDBY", variable.c_str()),"", "same hist");
  TH1F *hCRY = (TH1F*)gDirectory->Get("hCRY");
  TH1F *hDBY = (TH1F*)gDirectory->Get("hDBY");

  hCRY->SetLineColor(kBlue);
  hDBY->SetLineColor(kRed);
  hCRY->SetNormFactor(1.);
  hDBY->SetNormFactor(1.);
  hCRY->GetYaxis()->SetCanExtend(true);

  double maxCRY = hCRY->GetBinCenter(hCRY->GetMaximumBin());
  double maxDBY = hCRY->GetBinCenter(hDBY->GetMaximumBin());

  if (maxDBY > maxCRY)
    {
      cout <<"maxCRY = " << maxCRY << " and maxDBY = " << maxDBY<< endl;
      hCRY->SetMaximum(25 * maxDBY);
    }
  

  /*double maxCry = hCRY->GetXaxis()->GetBinCenter(hCRY->GetMaximumBin());
  double maxDby = hDBY->GetXaxis()->GetBinCenter(hDBY->GetMaximumBin());

  if(maxCry > maxDby){
    hCRY->Draw(); 
    hDBY->Draw("same");
   }
  else{
     hDBY->Draw();
     hCRY->Draw("same"); 
     }*/
    
  

  TLegend *leg = new TLegend(0.0,0.9,0.1,1.0);
  leg->AddEntry(hCRY,Form("CRY: %d", (int)hCRY->GetEntries()),"l");
  leg->AddEntry(hDBY,Form("DBY: %d", (int)hDBY->GetEntries()),"l");
  leg->Draw("same");

  std::string plot_name;
  plot_name.reserve(variable.size());
  for(size_t i = 0; i < variable.size(); ++i)
    if(variable[i] != ']' && variable[i] != '[' && variable[i] != '_') plot_name += variable[i];

  c1->SaveAs(Form("plots/%s_%d.pdf", plot_name.c_str(), cut_version));
  c1->SetLogy();
  c1->SaveAs(Form("plots/%s_%d_log.pdf", plot_name.c_str(), cut_version));
  hCRY->Delete();
  hDBY->Delete();

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

  std::string signal_all = all_cuts_MDC+"&& crvinfomc._z[0]>13000";
  // std::string signal_all = all_cuts_MDC;

  string buff = "";

  // Compare_two_plots("ncrv", signal_all.c_str(), 1);
  // Compare_two_plots("demc.pdg", signal_all.c_str(), 1);
  // Compare_two_plots("crvinfomc._z[0]", signal_all.c_str(), 1);

  // std::vector<std::string> cuts = { "de.mom>0", all_cuts_MDC, all_cuts_CD3, trk_cuts_CD3, trk_cuts_MDC, pitch_angle,min_trans_R, max_trans_R, signal_pid};
  std::vector<std::string> cuts = {all_cuts_MDC};
  for (int n=0; n<cuts.size(); n++){
    std::cout << "Cut: " << cuts[n] << std::endl;

    // Compare_two_graphs("crvinfomc._x[0]:crvinfomc._z[0]", "xz", cuts[n].c_str(), signal_all.c_str(), n);
    // Compare_two_graphs("crvinfomc._y[0]:crvinfomc._z[0]", "yz", cuts[n].c_str(), signal_all.c_str(), n);

    // Compare_two_plots("evtinfo.subrunid+100000*evtinfo.eventid",cuts[n].c_str(), n);
    Compare_two_plots("de.mom",cuts[n].c_str(), n);
    // Compare_two_plots("demc.pmom", cuts[n].c_str(), n);
    Compare_two_plots("de.td", cuts[n].c_str(), n);
    Compare_two_plots("de.d0", cuts[n].c_str(), n);
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
    // Compare_two_plots("de.t0err", cuts[n].c_str(), n);
    // Compare_two_plots("demc.pdg", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ppdg", cuts[n].c_str(), n);
    Compare_two_plots("demc.prpdg", cuts[n].c_str(), n);
    // Compare_two_plots("demc.nambig", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ngood", cuts[n].c_str(), n);
    // Compare_two_plots("demc.nactive", cuts[n].c_str(), n);
    // Compare_two_plots("demc.nhits", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ndigigood", cuts[n].c_str(), n);
    // Compare_two_plots("demc.ndigi", cuts[n].c_str(), n);    
    Compare_two_plots("crvinfomc._x[0]", cuts[n].c_str(), n);
    Compare_two_plots("crvinfomc._y[0]", cuts[n].c_str(), n);
    Compare_two_plots("crvinfomc._z[0]", cuts[n].c_str(), n);

    //std::cin >> buff;
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

