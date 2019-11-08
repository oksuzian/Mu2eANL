void Bootstrap_test(){

  TFile *file = TFile::Open("/mu2e/data/users/oksuzian/nts.owner.trkana-reco.cosmic-resample.round4.root");
  TH1F *h = new TH1F("h", "events", 1e6, 0, 1e6);
  TTree *tree = (TTree*) file->Get("TrkAnaNeg/trkana");  
  //  tree->Draw("evtinfo.eventid>>h(1e6, 0, 1e6)","detrkpid.mvaout>0.9 && @crvinfo.size()<1");
  //  tree->Draw("evtinfo.eventid>>+h","");
  tree->Draw("evtinfo.eventid>>+h","detrkpid.mvaout>0.9 && @crvinfo.size()<1");

  TH1F *hMean1 = new TH1F("hMean1", "hMean1", 100, 100, 400);
  TH1F *hMean2 = new TH1F("hMean2", "hMean2", 100, 100, 400);
  TH1F *hMean3 = new TH1F("hMean3", "hMean3", 100, 100, 400);
  TH1F *hMean4 = new TH1F("hMean4", "hMean4", 100, 100, 400);
  hMean1->SetLineColor(2);
  hMean2->SetLineColor(3);
  hMean3->SetLineColor(4);

  TH1F *hPerfectMean = new TH1F("hPerfectMean", "hPerfectMean", 100, 100, 400);
  const int totEve = 134952;
  const int totSample = 3;
  const int totEveSample = 134952/3;

  for (int i=0; i<10000; i++){

    int nEve1, nEve2, nEve3, nEve4;
    nEve1 = nEve2 = nEve3 = nEve4 = 0;    
    for (int k=0; k<totEve; k++){
      int ibin = (int)gRandom->Uniform(0, totEveSample);
      nEve1 = nEve1 + h->GetBinContent(ibin);
      ibin = (int)gRandom->Uniform(totEveSample, 2*totEveSample);
      nEve2 = nEve2 + h->GetBinContent(ibin);
      ibin = (int)gRandom->Uniform(2*totEveSample, 3*totEveSample);
      nEve3 = nEve3 + h->GetBinContent(ibin);
      ibin = (int)gRandom->Uniform(0, 3*totEveSample);
      nEve4 = nEve4 + h->GetBinContent(ibin);
    }

    hMean1->Fill(nEve1);
    hMean2->Fill(nEve2);
    hMean3->Fill(nEve3);
    hMean4->Fill(nEve4);
    hPerfectMean->Fill(gRandom->Poisson(255));
    //    hPerfectMean->Fill(gRandom->Poisson(2394/2.));
  }
  hPerfectMean->Draw();
  // hMean1->Draw("same");
  // hMean2->Draw("same");
  // hMean3->Draw("same");
  hMean4->Draw("same");

}
